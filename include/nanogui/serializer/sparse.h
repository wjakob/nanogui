/*
    nanogui/serializer/sparse.h -- serialization support for sparse matrices

    NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/serializer/core.h>
#include <Eigen/SparseCore>

NAMESPACE_BEGIN(nanogui)
NAMESPACE_BEGIN(detail)

// bypass template specializations
#ifndef DOXYGEN_SHOULD_SKIP_THIS

template <typename Scalar, int Options, typename Index>
struct serialization_helper<Eigen::SparseMatrix<Scalar, Options, Index>> {
    typedef Eigen::SparseMatrix<Scalar, Options, Index> Matrix;
    typedef Eigen::Triplet<Scalar> Triplet;

    static std::string type_id() {
        return "S" + serialization_helper<Index>::type_id() + serialization_helper<Scalar>::type_id();
    }

    static void write(Serializer &s, const Matrix *value, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            size_t index = 0;
            std::vector<std::pair<Index, Index>> positions(value->nonZeros());
            std::vector<Scalar> coeffs(value->nonZeros());

            for (int k = 0; k < value->outerSize(); ++k) {
                for (typename Matrix::InnerIterator it(*value, k); it; ++it) {
                    positions[index] = std::make_pair(it.row(), it.col());
                    coeffs[index] = it.value();
                    index++;
                }
            }

            Index rows = value->rows(), cols = value->cols();
            s.write(&rows, sizeof(Index));
            s.write(&cols, sizeof(Index));
            serialization_helper<std::vector<std::pair<Index, Index>>>::write(s, &positions, 1);
            serialization_helper<std::vector<Scalar>>::write(s, &coeffs, 1);

            ++value;
        }
    }

    static void read(Serializer &s, Matrix *value, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            Index rows, cols;
            s.read(&rows, sizeof(Index));
            s.read(&cols, sizeof(Index));

            std::vector<std::pair<Index, Index>> positions;
            std::vector<Scalar> coeffs;
            serialization_helper<std::vector<std::pair<Index, Index>>>::read(s, &positions, 1);
            serialization_helper<std::vector<Scalar>>::read(s, &coeffs, 1);

            if (coeffs.size() != positions.size())
                throw std::runtime_error("Encountered corrupt data while unserializing sparse matrix!");

            std::vector<Triplet> triplets(coeffs.size());

            for (uint32_t i=0; i<coeffs.size(); ++i)
                triplets[i] = Triplet(positions[i].first, positions[i].second, coeffs[i]);

            value->resize(rows, cols);
            value->setFromTriplets(triplets.begin(), triplets.end());

            ++value;
        }
    }
};

#endif // DOXYGEN_SHOULD_SKIP_THIS

NAMESPACE_END(detail)
NAMESPACE_END(nanogui)
