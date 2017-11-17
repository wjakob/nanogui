/*
     NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
     The widget drawing code is based on the NanoVG demo application
     by Mikko Mononen.

     All rights reserved. Use of this source code is governed by a
     BSD-style license that can be found in the LICENSE.txt file.
 */

/* Developer note: need to make a change to this file?
 * Please raise an Issue on GitHub describing what needs to change.  This file
 * was generated, so the scripts that generated it need to update as well.
 */

/**
 * \file nanogui/entypo.h
 *
 * \brief This is a list of icon codes for the ``entypo.ttf`` font by Daniel Bruce.
 *
 * \rst
 *
 * This file defines the full listing of `Entypo <http://www.entypo.com/>`_
 * icons available in NanoGUI.  Please note that if viewing the documentation
 * on the web, your browser may display the icons differently than what they
 * look like in NanoGUI.  Run the one of the :ref:`nanogui_example_icons`
 * executables to see what they all look like in NanoGUI.
 *
 * .. note::
 *
 *    Constants you may have used in the past may no longer exist, e.g.
 *    the name may have changed slightly.  For example, ``ENTYPO_ICON_CIRCLED_HELP``
 *    is renamed to ``ENTYPO_ICON_HELP_WITH_CIRCLE``.
 *
 * .. warning::
 *
 *    Some icons have a "small" variant, e.g. ``ENTYPO_ICON_CHEVRON_SMALL_LEFT``
 *    is smaller than ``ENTYPO_ICON_CHEVRON_LEFT``.  While these "small" icons
 *    can be used, they may not be positioned correctly.  If you experience this
 *    you can, instead of using the "small" variant, use the "regular" icon and
 *    call the :func:`nanogui::Widget::setIconExtraScale` function with a value
 *    of **less than** ``1.0`` to scale the icon size down.
 *
 * .. tip::
 *
 *    In C++, ``#include <nanogui/entypo.h>`` to gain access to the ``#define``
 *    shown in these docs.  In Python, ``from nanogui import entypo``.  So in the
 *    below table, when you see ``ENTYPO_ICON_FLOW_TREE`` that is for C++, and
 *    when you see ``ICON_FLOW_TREE``, that is for Python, and you would access
 *    it using ``entypo.ICON_FLOW_TREE``.
 *
 * The following icons are available:
 *
 * .. raw:: html
 *
 *    <center>
 *      <div class="wy-table-responsive">
 *        <table class="docutils" border=1>
 *          <colgroup>
 *            <col width="55%" align="center"/>
 *            <col width="10%" align="center" />
 *            <col width="35%" align="center"/>
 *          </colgroup>
 *          <thead valign="bottom">
 *            <tr class="row-odd">
 *              <th class="head" align="center">C++ Definition</th>
 *              <th class="head" align="center">Icon</th>
 *              <th class="head" align="center">Python Definition</th>
 *            </tr>
 *          </thead>
 *          <tbody valign="top">
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_500PX</code></td>
 *              <td align="center"><span class="entypo-icon-500px"></span></td>
 *              <td align="left"><code>ICON_500PX</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_500PX_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-500px-with-circle"></span></td>
 *              <td align="left"><code>ICON_500PX_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ADD_TO_LIST</code></td>
 *              <td align="center"><span class="entypo-icon-add-to-list"></span></td>
 *              <td align="left"><code>ICON_ADD_TO_LIST</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ADD_USER</code></td>
 *              <td align="center"><span class="entypo-icon-add-user"></span></td>
 *              <td align="left"><code>ICON_ADD_USER</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ADDRESS</code></td>
 *              <td align="center"><span class="entypo-icon-address"></span></td>
 *              <td align="left"><code>ICON_ADDRESS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ADJUST</code></td>
 *              <td align="center"><span class="entypo-icon-adjust"></span></td>
 *              <td align="left"><code>ICON_ADJUST</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_AIR</code></td>
 *              <td align="center"><span class="entypo-icon-air"></span></td>
 *              <td align="left"><code>ICON_AIR</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_AIRCRAFT</code></td>
 *              <td align="center"><span class="entypo-icon-aircraft"></span></td>
 *              <td align="left"><code>ICON_AIRCRAFT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_AIRCRAFT_LANDING</code></td>
 *              <td align="center"><span class="entypo-icon-aircraft-landing"></span></td>
 *              <td align="left"><code>ICON_AIRCRAFT_LANDING</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_AIRCRAFT_TAKE_OFF</code></td>
 *              <td align="center"><span class="entypo-icon-aircraft-take-off"></span></td>
 *              <td align="left"><code>ICON_AIRCRAFT_TAKE_OFF</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ALIGN_BOTTOM</code></td>
 *              <td align="center"><span class="entypo-icon-align-bottom"></span></td>
 *              <td align="left"><code>ICON_ALIGN_BOTTOM</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ALIGN_HORIZONTAL_MIDDLE</code></td>
 *              <td align="center"><span class="entypo-icon-align-horizontal-middle"></span></td>
 *              <td align="left"><code>ICON_ALIGN_HORIZONTAL_MIDDLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ALIGN_LEFT</code></td>
 *              <td align="center"><span class="entypo-icon-align-left"></span></td>
 *              <td align="left"><code>ICON_ALIGN_LEFT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ALIGN_RIGHT</code></td>
 *              <td align="center"><span class="entypo-icon-align-right"></span></td>
 *              <td align="left"><code>ICON_ALIGN_RIGHT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ALIGN_TOP</code></td>
 *              <td align="center"><span class="entypo-icon-align-top"></span></td>
 *              <td align="left"><code>ICON_ALIGN_TOP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ALIGN_VERTICAL_MIDDLE</code></td>
 *              <td align="center"><span class="entypo-icon-align-vertical-middle"></span></td>
 *              <td align="left"><code>ICON_ALIGN_VERTICAL_MIDDLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_APP_STORE</code></td>
 *              <td align="center"><span class="entypo-icon-app-store"></span></td>
 *              <td align="left"><code>ICON_APP_STORE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ARCHIVE</code></td>
 *              <td align="center"><span class="entypo-icon-archive"></span></td>
 *              <td align="left"><code>ICON_ARCHIVE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_AREA_GRAPH</code></td>
 *              <td align="center"><span class="entypo-icon-area-graph"></span></td>
 *              <td align="left"><code>ICON_AREA_GRAPH</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_BOLD_DOWN</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-bold-down"></span></td>
 *              <td align="left"><code>ICON_ARROW_BOLD_DOWN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_BOLD_LEFT</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-bold-left"></span></td>
 *              <td align="left"><code>ICON_ARROW_BOLD_LEFT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_BOLD_RIGHT</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-bold-right"></span></td>
 *              <td align="left"><code>ICON_ARROW_BOLD_RIGHT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_BOLD_UP</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-bold-up"></span></td>
 *              <td align="left"><code>ICON_ARROW_BOLD_UP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_DOWN</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-down"></span></td>
 *              <td align="left"><code>ICON_ARROW_DOWN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_LEFT</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-left"></span></td>
 *              <td align="left"><code>ICON_ARROW_LEFT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_LONG_DOWN</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-long-down"></span></td>
 *              <td align="left"><code>ICON_ARROW_LONG_DOWN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_LONG_LEFT</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-long-left"></span></td>
 *              <td align="left"><code>ICON_ARROW_LONG_LEFT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_LONG_RIGHT</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-long-right"></span></td>
 *              <td align="left"><code>ICON_ARROW_LONG_RIGHT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_LONG_UP</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-long-up"></span></td>
 *              <td align="left"><code>ICON_ARROW_LONG_UP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_RIGHT</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-right"></span></td>
 *              <td align="left"><code>ICON_ARROW_RIGHT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_UP</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-up"></span></td>
 *              <td align="left"><code>ICON_ARROW_UP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_WITH_CIRCLE_DOWN</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-with-circle-down"></span></td>
 *              <td align="left"><code>ICON_ARROW_WITH_CIRCLE_DOWN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_WITH_CIRCLE_LEFT</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-with-circle-left"></span></td>
 *              <td align="left"><code>ICON_ARROW_WITH_CIRCLE_LEFT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_WITH_CIRCLE_RIGHT</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-with-circle-right"></span></td>
 *              <td align="left"><code>ICON_ARROW_WITH_CIRCLE_RIGHT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ARROW_WITH_CIRCLE_UP</code></td>
 *              <td align="center"><span class="entypo-icon-arrow-with-circle-up"></span></td>
 *              <td align="left"><code>ICON_ARROW_WITH_CIRCLE_UP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ATTACHMENT</code></td>
 *              <td align="center"><span class="entypo-icon-attachment"></span></td>
 *              <td align="left"><code>ICON_ATTACHMENT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_AWARENESS_RIBBON</code></td>
 *              <td align="center"><span class="entypo-icon-awareness-ribbon"></span></td>
 *              <td align="left"><code>ICON_AWARENESS_RIBBON</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_BACK</code></td>
 *              <td align="center"><span class="entypo-icon-back"></span></td>
 *              <td align="left"><code>ICON_BACK</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_BACK_IN_TIME</code></td>
 *              <td align="center"><span class="entypo-icon-back-in-time"></span></td>
 *              <td align="left"><code>ICON_BACK_IN_TIME</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_BAIDU</code></td>
 *              <td align="center"><span class="entypo-icon-baidu"></span></td>
 *              <td align="left"><code>ICON_BAIDU</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_BAR_GRAPH</code></td>
 *              <td align="center"><span class="entypo-icon-bar-graph"></span></td>
 *              <td align="left"><code>ICON_BAR_GRAPH</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_BASECAMP</code></td>
 *              <td align="center"><span class="entypo-icon-basecamp"></span></td>
 *              <td align="left"><code>ICON_BASECAMP</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_BATTERY</code></td>
 *              <td align="center"><span class="entypo-icon-battery"></span></td>
 *              <td align="left"><code>ICON_BATTERY</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_BEAMED_NOTE</code></td>
 *              <td align="center"><span class="entypo-icon-beamed-note"></span></td>
 *              <td align="left"><code>ICON_BEAMED_NOTE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_BEHANCE</code></td>
 *              <td align="center"><span class="entypo-icon-behance"></span></td>
 *              <td align="left"><code>ICON_BEHANCE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_BELL</code></td>
 *              <td align="center"><span class="entypo-icon-bell"></span></td>
 *              <td align="left"><code>ICON_BELL</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_BLACKBOARD</code></td>
 *              <td align="center"><span class="entypo-icon-blackboard"></span></td>
 *              <td align="left"><code>ICON_BLACKBOARD</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_BLOCK</code></td>
 *              <td align="center"><span class="entypo-icon-block"></span></td>
 *              <td align="left"><code>ICON_BLOCK</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_BOOK</code></td>
 *              <td align="center"><span class="entypo-icon-book"></span></td>
 *              <td align="left"><code>ICON_BOOK</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_BOOKMARK</code></td>
 *              <td align="center"><span class="entypo-icon-bookmark"></span></td>
 *              <td align="left"><code>ICON_BOOKMARK</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_BOOKMARKS</code></td>
 *              <td align="center"><span class="entypo-icon-bookmarks"></span></td>
 *              <td align="left"><code>ICON_BOOKMARKS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_BOWL</code></td>
 *              <td align="center"><span class="entypo-icon-bowl"></span></td>
 *              <td align="left"><code>ICON_BOWL</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_BOX</code></td>
 *              <td align="center"><span class="entypo-icon-box"></span></td>
 *              <td align="left"><code>ICON_BOX</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_BRIEFCASE</code></td>
 *              <td align="center"><span class="entypo-icon-briefcase"></span></td>
 *              <td align="left"><code>ICON_BRIEFCASE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_BROWSER</code></td>
 *              <td align="center"><span class="entypo-icon-browser"></span></td>
 *              <td align="left"><code>ICON_BROWSER</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_BRUSH</code></td>
 *              <td align="center"><span class="entypo-icon-brush"></span></td>
 *              <td align="left"><code>ICON_BRUSH</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_BUCKET</code></td>
 *              <td align="center"><span class="entypo-icon-bucket"></span></td>
 *              <td align="left"><code>ICON_BUCKET</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_BUG</code></td>
 *              <td align="center"><span class="entypo-icon-bug"></span></td>
 *              <td align="left"><code>ICON_BUG</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CAKE</code></td>
 *              <td align="center"><span class="entypo-icon-cake"></span></td>
 *              <td align="left"><code>ICON_CAKE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CALCULATOR</code></td>
 *              <td align="center"><span class="entypo-icon-calculator"></span></td>
 *              <td align="left"><code>ICON_CALCULATOR</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CALENDAR</code></td>
 *              <td align="center"><span class="entypo-icon-calendar"></span></td>
 *              <td align="left"><code>ICON_CALENDAR</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CAMERA</code></td>
 *              <td align="center"><span class="entypo-icon-camera"></span></td>
 *              <td align="left"><code>ICON_CAMERA</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CCW</code></td>
 *              <td align="center"><span class="entypo-icon-ccw"></span></td>
 *              <td align="left"><code>ICON_CCW</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CHAT</code></td>
 *              <td align="center"><span class="entypo-icon-chat"></span></td>
 *              <td align="left"><code>ICON_CHAT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CHECK</code></td>
 *              <td align="center"><span class="entypo-icon-check"></span></td>
 *              <td align="left"><code>ICON_CHECK</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_DOWN</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-down"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_DOWN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_LEFT</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-left"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_LEFT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_RIGHT</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-right"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_RIGHT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_SMALL_DOWN</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-small-down"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_SMALL_DOWN</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_SMALL_LEFT</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-small-left"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_SMALL_LEFT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_SMALL_RIGHT</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-small-right"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_SMALL_RIGHT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_SMALL_UP</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-small-up"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_SMALL_UP</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_THIN_DOWN</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-thin-down"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_THIN_DOWN</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_THIN_LEFT</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-thin-left"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_THIN_LEFT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_THIN_RIGHT</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-thin-right"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_THIN_RIGHT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_THIN_UP</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-thin-up"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_THIN_UP</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_UP</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-up"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_UP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_WITH_CIRCLE_DOWN</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-with-circle-down"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_WITH_CIRCLE_DOWN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_WITH_CIRCLE_LEFT</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-with-circle-left"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_WITH_CIRCLE_LEFT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_WITH_CIRCLE_RIGHT</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-with-circle-right"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_WITH_CIRCLE_RIGHT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CHEVRON_WITH_CIRCLE_UP</code></td>
 *              <td align="center"><span class="entypo-icon-chevron-with-circle-up"></span></td>
 *              <td align="left"><code>ICON_CHEVRON_WITH_CIRCLE_UP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-circle"></span></td>
 *              <td align="left"><code>ICON_CIRCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CIRCLE_WITH_CROSS</code></td>
 *              <td align="center"><span class="entypo-icon-circle-with-cross"></span></td>
 *              <td align="left"><code>ICON_CIRCLE_WITH_CROSS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CIRCLE_WITH_MINUS</code></td>
 *              <td align="center"><span class="entypo-icon-circle-with-minus"></span></td>
 *              <td align="left"><code>ICON_CIRCLE_WITH_MINUS</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CIRCLE_WITH_PLUS</code></td>
 *              <td align="center"><span class="entypo-icon-circle-with-plus"></span></td>
 *              <td align="left"><code>ICON_CIRCLE_WITH_PLUS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CIRCULAR_GRAPH</code></td>
 *              <td align="center"><span class="entypo-icon-circular-graph"></span></td>
 *              <td align="left"><code>ICON_CIRCULAR_GRAPH</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CLAPPERBOARD</code></td>
 *              <td align="center"><span class="entypo-icon-clapperboard"></span></td>
 *              <td align="left"><code>ICON_CLAPPERBOARD</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CLASSIC_COMPUTER</code></td>
 *              <td align="center"><span class="entypo-icon-classic-computer"></span></td>
 *              <td align="left"><code>ICON_CLASSIC_COMPUTER</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CLIPBOARD</code></td>
 *              <td align="center"><span class="entypo-icon-clipboard"></span></td>
 *              <td align="left"><code>ICON_CLIPBOARD</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CLOCK</code></td>
 *              <td align="center"><span class="entypo-icon-clock"></span></td>
 *              <td align="left"><code>ICON_CLOCK</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CLOUD</code></td>
 *              <td align="center"><span class="entypo-icon-cloud"></span></td>
 *              <td align="left"><code>ICON_CLOUD</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CODE</code></td>
 *              <td align="center"><span class="entypo-icon-code"></span></td>
 *              <td align="left"><code>ICON_CODE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_COG</code></td>
 *              <td align="center"><span class="entypo-icon-cog"></span></td>
 *              <td align="left"><code>ICON_COG</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_COLOURS</code></td>
 *              <td align="center"><span class="entypo-icon-colours"></span></td>
 *              <td align="left"><code>ICON_COLOURS</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_COMPASS</code></td>
 *              <td align="center"><span class="entypo-icon-compass"></span></td>
 *              <td align="left"><code>ICON_COMPASS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CONTROLLER_FAST_BACKWARD</code></td>
 *              <td align="center"><span class="entypo-icon-controller-fast-backward"></span></td>
 *              <td align="left"><code>ICON_CONTROLLER_FAST_BACKWARD</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CONTROLLER_FAST_FORWARD</code></td>
 *              <td align="center"><span class="entypo-icon-controller-fast-forward"></span></td>
 *              <td align="left"><code>ICON_CONTROLLER_FAST_FORWARD</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CONTROLLER_JUMP_TO_START</code></td>
 *              <td align="center"><span class="entypo-icon-controller-jump-to-start"></span></td>
 *              <td align="left"><code>ICON_CONTROLLER_JUMP_TO_START</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CONTROLLER_NEXT</code></td>
 *              <td align="center"><span class="entypo-icon-controller-next"></span></td>
 *              <td align="left"><code>ICON_CONTROLLER_NEXT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CONTROLLER_PAUS</code></td>
 *              <td align="center"><span class="entypo-icon-controller-paus"></span></td>
 *              <td align="left"><code>ICON_CONTROLLER_PAUS</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CONTROLLER_PLAY</code></td>
 *              <td align="center"><span class="entypo-icon-controller-play"></span></td>
 *              <td align="left"><code>ICON_CONTROLLER_PLAY</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CONTROLLER_RECORD</code></td>
 *              <td align="center"><span class="entypo-icon-controller-record"></span></td>
 *              <td align="left"><code>ICON_CONTROLLER_RECORD</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CONTROLLER_STOP</code></td>
 *              <td align="center"><span class="entypo-icon-controller-stop"></span></td>
 *              <td align="left"><code>ICON_CONTROLLER_STOP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CONTROLLER_VOLUME</code></td>
 *              <td align="center"><span class="entypo-icon-controller-volume"></span></td>
 *              <td align="left"><code>ICON_CONTROLLER_VOLUME</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_COPY</code></td>
 *              <td align="center"><span class="entypo-icon-copy"></span></td>
 *              <td align="left"><code>ICON_COPY</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CREATIVE_CLOUD</code></td>
 *              <td align="center"><span class="entypo-icon-creative-cloud"></span></td>
 *              <td align="left"><code>ICON_CREATIVE_CLOUD</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CREATIVE_COMMONS</code></td>
 *              <td align="center"><span class="entypo-icon-creative-commons"></span></td>
 *              <td align="left"><code>ICON_CREATIVE_COMMONS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CREATIVE_COMMONS_ATTRIBUTION</code></td>
 *              <td align="center"><span class="entypo-icon-creative-commons-attribution"></span></td>
 *              <td align="left"><code>ICON_CREATIVE_COMMONS_ATTRIBUTION</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CREATIVE_COMMONS_NODERIVS</code></td>
 *              <td align="center"><span class="entypo-icon-creative-commons-noderivs"></span></td>
 *              <td align="left"><code>ICON_CREATIVE_COMMONS_NODERIVS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CREATIVE_COMMONS_NONCOMMERCIAL_EU</code></td>
 *              <td align="center"><span class="entypo-icon-creative-commons-noncommercial-eu"></span></td>
 *              <td align="left"><code>ICON_CREATIVE_COMMONS_NONCOMMERCIAL_EU</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CREATIVE_COMMONS_NONCOMMERCIAL_US</code></td>
 *              <td align="center"><span class="entypo-icon-creative-commons-noncommercial-us"></span></td>
 *              <td align="left"><code>ICON_CREATIVE_COMMONS_NONCOMMERCIAL_US</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CREATIVE_COMMONS_PUBLIC_DOMAIN</code></td>
 *              <td align="center"><span class="entypo-icon-creative-commons-public-domain"></span></td>
 *              <td align="left"><code>ICON_CREATIVE_COMMONS_PUBLIC_DOMAIN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CREATIVE_COMMONS_REMIX</code></td>
 *              <td align="center"><span class="entypo-icon-creative-commons-remix"></span></td>
 *              <td align="left"><code>ICON_CREATIVE_COMMONS_REMIX</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CREATIVE_COMMONS_SHARE</code></td>
 *              <td align="center"><span class="entypo-icon-creative-commons-share"></span></td>
 *              <td align="left"><code>ICON_CREATIVE_COMMONS_SHARE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CREATIVE_COMMONS_SHAREALIKE</code></td>
 *              <td align="center"><span class="entypo-icon-creative-commons-sharealike"></span></td>
 *              <td align="left"><code>ICON_CREATIVE_COMMONS_SHAREALIKE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CREDIT</code></td>
 *              <td align="center"><span class="entypo-icon-credit"></span></td>
 *              <td align="left"><code>ICON_CREDIT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CREDIT_CARD</code></td>
 *              <td align="center"><span class="entypo-icon-credit-card"></span></td>
 *              <td align="left"><code>ICON_CREDIT_CARD</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CROP</code></td>
 *              <td align="center"><span class="entypo-icon-crop"></span></td>
 *              <td align="left"><code>ICON_CROP</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CROSS</code></td>
 *              <td align="center"><span class="entypo-icon-cross"></span></td>
 *              <td align="left"><code>ICON_CROSS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CUP</code></td>
 *              <td align="center"><span class="entypo-icon-cup"></span></td>
 *              <td align="left"><code>ICON_CUP</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_CW</code></td>
 *              <td align="center"><span class="entypo-icon-cw"></span></td>
 *              <td align="left"><code>ICON_CW</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_CYCLE</code></td>
 *              <td align="center"><span class="entypo-icon-cycle"></span></td>
 *              <td align="left"><code>ICON_CYCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_DATABASE</code></td>
 *              <td align="center"><span class="entypo-icon-database"></span></td>
 *              <td align="left"><code>ICON_DATABASE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_DIAL_PAD</code></td>
 *              <td align="center"><span class="entypo-icon-dial-pad"></span></td>
 *              <td align="left"><code>ICON_DIAL_PAD</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_DIRECTION</code></td>
 *              <td align="center"><span class="entypo-icon-direction"></span></td>
 *              <td align="left"><code>ICON_DIRECTION</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_DOCUMENT</code></td>
 *              <td align="center"><span class="entypo-icon-document"></span></td>
 *              <td align="left"><code>ICON_DOCUMENT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_DOCUMENT_LANDSCAPE</code></td>
 *              <td align="center"><span class="entypo-icon-document-landscape"></span></td>
 *              <td align="left"><code>ICON_DOCUMENT_LANDSCAPE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_DOCUMENTS</code></td>
 *              <td align="center"><span class="entypo-icon-documents"></span></td>
 *              <td align="left"><code>ICON_DOCUMENTS</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_DOT_SINGLE</code></td>
 *              <td align="center"><span class="entypo-icon-dot-single"></span></td>
 *              <td align="left"><code>ICON_DOT_SINGLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_DOTS_THREE_HORIZONTAL</code></td>
 *              <td align="center"><span class="entypo-icon-dots-three-horizontal"></span></td>
 *              <td align="left"><code>ICON_DOTS_THREE_HORIZONTAL</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_DOTS_THREE_VERTICAL</code></td>
 *              <td align="center"><span class="entypo-icon-dots-three-vertical"></span></td>
 *              <td align="left"><code>ICON_DOTS_THREE_VERTICAL</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_DOTS_TWO_HORIZONTAL</code></td>
 *              <td align="center"><span class="entypo-icon-dots-two-horizontal"></span></td>
 *              <td align="left"><code>ICON_DOTS_TWO_HORIZONTAL</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_DOTS_TWO_VERTICAL</code></td>
 *              <td align="center"><span class="entypo-icon-dots-two-vertical"></span></td>
 *              <td align="left"><code>ICON_DOTS_TWO_VERTICAL</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_DOWNLOAD</code></td>
 *              <td align="center"><span class="entypo-icon-download"></span></td>
 *              <td align="left"><code>ICON_DOWNLOAD</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_DRIBBBLE</code></td>
 *              <td align="center"><span class="entypo-icon-dribbble"></span></td>
 *              <td align="left"><code>ICON_DRIBBBLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_DRIBBBLE_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-dribbble-with-circle"></span></td>
 *              <td align="left"><code>ICON_DRIBBBLE_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_DRINK</code></td>
 *              <td align="center"><span class="entypo-icon-drink"></span></td>
 *              <td align="left"><code>ICON_DRINK</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_DRIVE</code></td>
 *              <td align="center"><span class="entypo-icon-drive"></span></td>
 *              <td align="left"><code>ICON_DRIVE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_DROP</code></td>
 *              <td align="center"><span class="entypo-icon-drop"></span></td>
 *              <td align="left"><code>ICON_DROP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_DROPBOX</code></td>
 *              <td align="center"><span class="entypo-icon-dropbox"></span></td>
 *              <td align="left"><code>ICON_DROPBOX</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_EDIT</code></td>
 *              <td align="center"><span class="entypo-icon-edit"></span></td>
 *              <td align="left"><code>ICON_EDIT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_EMAIL</code></td>
 *              <td align="center"><span class="entypo-icon-email"></span></td>
 *              <td align="left"><code>ICON_EMAIL</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_EMOJI_FLIRT</code></td>
 *              <td align="center"><span class="entypo-icon-emoji-flirt"></span></td>
 *              <td align="left"><code>ICON_EMOJI_FLIRT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_EMOJI_HAPPY</code></td>
 *              <td align="center"><span class="entypo-icon-emoji-happy"></span></td>
 *              <td align="left"><code>ICON_EMOJI_HAPPY</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_EMOJI_NEUTRAL</code></td>
 *              <td align="center"><span class="entypo-icon-emoji-neutral"></span></td>
 *              <td align="left"><code>ICON_EMOJI_NEUTRAL</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_EMOJI_SAD</code></td>
 *              <td align="center"><span class="entypo-icon-emoji-sad"></span></td>
 *              <td align="left"><code>ICON_EMOJI_SAD</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ERASE</code></td>
 *              <td align="center"><span class="entypo-icon-erase"></span></td>
 *              <td align="left"><code>ICON_ERASE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ERASER</code></td>
 *              <td align="center"><span class="entypo-icon-eraser"></span></td>
 *              <td align="left"><code>ICON_ERASER</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_EVERNOTE</code></td>
 *              <td align="center"><span class="entypo-icon-evernote"></span></td>
 *              <td align="left"><code>ICON_EVERNOTE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_EXPORT</code></td>
 *              <td align="center"><span class="entypo-icon-export"></span></td>
 *              <td align="left"><code>ICON_EXPORT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_EYE</code></td>
 *              <td align="center"><span class="entypo-icon-eye"></span></td>
 *              <td align="left"><code>ICON_EYE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_EYE_WITH_LINE</code></td>
 *              <td align="center"><span class="entypo-icon-eye-with-line"></span></td>
 *              <td align="left"><code>ICON_EYE_WITH_LINE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_FACEBOOK</code></td>
 *              <td align="center"><span class="entypo-icon-facebook"></span></td>
 *              <td align="left"><code>ICON_FACEBOOK</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_FACEBOOK_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-facebook-with-circle"></span></td>
 *              <td align="left"><code>ICON_FACEBOOK_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_FEATHER</code></td>
 *              <td align="center"><span class="entypo-icon-feather"></span></td>
 *              <td align="left"><code>ICON_FEATHER</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_FINGERPRINT</code></td>
 *              <td align="center"><span class="entypo-icon-fingerprint"></span></td>
 *              <td align="left"><code>ICON_FINGERPRINT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_FLAG</code></td>
 *              <td align="center"><span class="entypo-icon-flag"></span></td>
 *              <td align="left"><code>ICON_FLAG</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_FLASH</code></td>
 *              <td align="center"><span class="entypo-icon-flash"></span></td>
 *              <td align="left"><code>ICON_FLASH</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_FLASHLIGHT</code></td>
 *              <td align="center"><span class="entypo-icon-flashlight"></span></td>
 *              <td align="left"><code>ICON_FLASHLIGHT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_FLAT_BRUSH</code></td>
 *              <td align="center"><span class="entypo-icon-flat-brush"></span></td>
 *              <td align="left"><code>ICON_FLAT_BRUSH</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_FLATTR</code></td>
 *              <td align="center"><span class="entypo-icon-flattr"></span></td>
 *              <td align="left"><code>ICON_FLATTR</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_FLICKR</code></td>
 *              <td align="center"><span class="entypo-icon-flickr"></span></td>
 *              <td align="left"><code>ICON_FLICKR</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_FLICKR_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-flickr-with-circle"></span></td>
 *              <td align="left"><code>ICON_FLICKR_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_FLOW_BRANCH</code></td>
 *              <td align="center"><span class="entypo-icon-flow-branch"></span></td>
 *              <td align="left"><code>ICON_FLOW_BRANCH</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_FLOW_CASCADE</code></td>
 *              <td align="center"><span class="entypo-icon-flow-cascade"></span></td>
 *              <td align="left"><code>ICON_FLOW_CASCADE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_FLOW_LINE</code></td>
 *              <td align="center"><span class="entypo-icon-flow-line"></span></td>
 *              <td align="left"><code>ICON_FLOW_LINE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_FLOW_PARALLEL</code></td>
 *              <td align="center"><span class="entypo-icon-flow-parallel"></span></td>
 *              <td align="left"><code>ICON_FLOW_PARALLEL</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_FLOW_TREE</code></td>
 *              <td align="center"><span class="entypo-icon-flow-tree"></span></td>
 *              <td align="left"><code>ICON_FLOW_TREE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_FLOWER</code></td>
 *              <td align="center"><span class="entypo-icon-flower"></span></td>
 *              <td align="left"><code>ICON_FLOWER</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_FOLDER</code></td>
 *              <td align="center"><span class="entypo-icon-folder"></span></td>
 *              <td align="left"><code>ICON_FOLDER</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_FOLDER_IMAGES</code></td>
 *              <td align="center"><span class="entypo-icon-folder-images"></span></td>
 *              <td align="left"><code>ICON_FOLDER_IMAGES</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_FOLDER_MUSIC</code></td>
 *              <td align="center"><span class="entypo-icon-folder-music"></span></td>
 *              <td align="left"><code>ICON_FOLDER_MUSIC</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_FOLDER_VIDEO</code></td>
 *              <td align="center"><span class="entypo-icon-folder-video"></span></td>
 *              <td align="left"><code>ICON_FOLDER_VIDEO</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_FORWARD</code></td>
 *              <td align="center"><span class="entypo-icon-forward"></span></td>
 *              <td align="left"><code>ICON_FORWARD</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_FOURSQUARE</code></td>
 *              <td align="center"><span class="entypo-icon-foursquare"></span></td>
 *              <td align="left"><code>ICON_FOURSQUARE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_FUNNEL</code></td>
 *              <td align="center"><span class="entypo-icon-funnel"></span></td>
 *              <td align="left"><code>ICON_FUNNEL</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_GAME_CONTROLLER</code></td>
 *              <td align="center"><span class="entypo-icon-game-controller"></span></td>
 *              <td align="left"><code>ICON_GAME_CONTROLLER</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_GAUGE</code></td>
 *              <td align="center"><span class="entypo-icon-gauge"></span></td>
 *              <td align="left"><code>ICON_GAUGE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_GITHUB</code></td>
 *              <td align="center"><span class="entypo-icon-github"></span></td>
 *              <td align="left"><code>ICON_GITHUB</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_GITHUB_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-github-with-circle"></span></td>
 *              <td align="left"><code>ICON_GITHUB_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_GLOBE</code></td>
 *              <td align="center"><span class="entypo-icon-globe"></span></td>
 *              <td align="left"><code>ICON_GLOBE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_GOOGLE_DRIVE</code></td>
 *              <td align="center"><span class="entypo-icon-google-drive"></span></td>
 *              <td align="left"><code>ICON_GOOGLE_DRIVE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_GOOGLE_HANGOUTS</code></td>
 *              <td align="center"><span class="entypo-icon-google-hangouts"></span></td>
 *              <td align="left"><code>ICON_GOOGLE_HANGOUTS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_GOOGLE_PLAY</code></td>
 *              <td align="center"><span class="entypo-icon-google-play"></span></td>
 *              <td align="left"><code>ICON_GOOGLE_PLAY</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_GOOGLE_PLUS</code></td>
 *              <td align="center"><span class="entypo-icon-google-plus"></span></td>
 *              <td align="left"><code>ICON_GOOGLE_PLUS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_GOOGLE_PLUS_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-google-plus-with-circle"></span></td>
 *              <td align="left"><code>ICON_GOOGLE_PLUS_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_GRADUATION_CAP</code></td>
 *              <td align="center"><span class="entypo-icon-graduation-cap"></span></td>
 *              <td align="left"><code>ICON_GRADUATION_CAP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_GRID</code></td>
 *              <td align="center"><span class="entypo-icon-grid"></span></td>
 *              <td align="left"><code>ICON_GRID</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_GROOVESHARK</code></td>
 *              <td align="center"><span class="entypo-icon-grooveshark"></span></td>
 *              <td align="left"><code>ICON_GROOVESHARK</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_HAIR_CROSS</code></td>
 *              <td align="center"><span class="entypo-icon-hair-cross"></span></td>
 *              <td align="left"><code>ICON_HAIR_CROSS</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_HAND</code></td>
 *              <td align="center"><span class="entypo-icon-hand"></span></td>
 *              <td align="left"><code>ICON_HAND</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_HEART</code></td>
 *              <td align="center"><span class="entypo-icon-heart"></span></td>
 *              <td align="left"><code>ICON_HEART</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_HEART_OUTLINED</code></td>
 *              <td align="center"><span class="entypo-icon-heart-outlined"></span></td>
 *              <td align="left"><code>ICON_HEART_OUTLINED</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_HELP</code></td>
 *              <td align="center"><span class="entypo-icon-help"></span></td>
 *              <td align="left"><code>ICON_HELP</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_HELP_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-help-with-circle"></span></td>
 *              <td align="left"><code>ICON_HELP_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_HOME</code></td>
 *              <td align="center"><span class="entypo-icon-home"></span></td>
 *              <td align="left"><code>ICON_HOME</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_HOUR_GLASS</code></td>
 *              <td align="center"><span class="entypo-icon-hour-glass"></span></td>
 *              <td align="left"><code>ICON_HOUR_GLASS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_HOUZZ</code></td>
 *              <td align="center"><span class="entypo-icon-houzz"></span></td>
 *              <td align="left"><code>ICON_HOUZZ</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ICLOUD</code></td>
 *              <td align="center"><span class="entypo-icon-icloud"></span></td>
 *              <td align="left"><code>ICON_ICLOUD</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_IMAGE</code></td>
 *              <td align="center"><span class="entypo-icon-image"></span></td>
 *              <td align="left"><code>ICON_IMAGE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_IMAGE_INVERTED</code></td>
 *              <td align="center"><span class="entypo-icon-image-inverted"></span></td>
 *              <td align="left"><code>ICON_IMAGE_INVERTED</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_IMAGES</code></td>
 *              <td align="center"><span class="entypo-icon-images"></span></td>
 *              <td align="left"><code>ICON_IMAGES</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_INBOX</code></td>
 *              <td align="center"><span class="entypo-icon-inbox"></span></td>
 *              <td align="left"><code>ICON_INBOX</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_INFINITY</code></td>
 *              <td align="center"><span class="entypo-icon-infinity"></span></td>
 *              <td align="left"><code>ICON_INFINITY</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_INFO</code></td>
 *              <td align="center"><span class="entypo-icon-info"></span></td>
 *              <td align="left"><code>ICON_INFO</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_INFO_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-info-with-circle"></span></td>
 *              <td align="left"><code>ICON_INFO_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_INSTAGRAM</code></td>
 *              <td align="center"><span class="entypo-icon-instagram"></span></td>
 *              <td align="left"><code>ICON_INSTAGRAM</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_INSTAGRAM_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-instagram-with-circle"></span></td>
 *              <td align="left"><code>ICON_INSTAGRAM_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_INSTALL</code></td>
 *              <td align="center"><span class="entypo-icon-install"></span></td>
 *              <td align="left"><code>ICON_INSTALL</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_KEY</code></td>
 *              <td align="center"><span class="entypo-icon-key"></span></td>
 *              <td align="left"><code>ICON_KEY</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_KEYBOARD</code></td>
 *              <td align="center"><span class="entypo-icon-keyboard"></span></td>
 *              <td align="left"><code>ICON_KEYBOARD</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_LAB_FLASK</code></td>
 *              <td align="center"><span class="entypo-icon-lab-flask"></span></td>
 *              <td align="left"><code>ICON_LAB_FLASK</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_LANDLINE</code></td>
 *              <td align="center"><span class="entypo-icon-landline"></span></td>
 *              <td align="left"><code>ICON_LANDLINE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_LANGUAGE</code></td>
 *              <td align="center"><span class="entypo-icon-language"></span></td>
 *              <td align="left"><code>ICON_LANGUAGE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_LAPTOP</code></td>
 *              <td align="center"><span class="entypo-icon-laptop"></span></td>
 *              <td align="left"><code>ICON_LAPTOP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_LASTFM</code></td>
 *              <td align="center"><span class="entypo-icon-lastfm"></span></td>
 *              <td align="left"><code>ICON_LASTFM</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_LASTFM_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-lastfm-with-circle"></span></td>
 *              <td align="left"><code>ICON_LASTFM_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_LAYERS</code></td>
 *              <td align="center"><span class="entypo-icon-layers"></span></td>
 *              <td align="left"><code>ICON_LAYERS</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_LEAF</code></td>
 *              <td align="center"><span class="entypo-icon-leaf"></span></td>
 *              <td align="left"><code>ICON_LEAF</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_LEVEL_DOWN</code></td>
 *              <td align="center"><span class="entypo-icon-level-down"></span></td>
 *              <td align="left"><code>ICON_LEVEL_DOWN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_LEVEL_UP</code></td>
 *              <td align="center"><span class="entypo-icon-level-up"></span></td>
 *              <td align="left"><code>ICON_LEVEL_UP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_LIFEBUOY</code></td>
 *              <td align="center"><span class="entypo-icon-lifebuoy"></span></td>
 *              <td align="left"><code>ICON_LIFEBUOY</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_LIGHT_BULB</code></td>
 *              <td align="center"><span class="entypo-icon-light-bulb"></span></td>
 *              <td align="left"><code>ICON_LIGHT_BULB</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_LIGHT_DOWN</code></td>
 *              <td align="center"><span class="entypo-icon-light-down"></span></td>
 *              <td align="left"><code>ICON_LIGHT_DOWN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_LIGHT_UP</code></td>
 *              <td align="center"><span class="entypo-icon-light-up"></span></td>
 *              <td align="left"><code>ICON_LIGHT_UP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_LINE_GRAPH</code></td>
 *              <td align="center"><span class="entypo-icon-line-graph"></span></td>
 *              <td align="left"><code>ICON_LINE_GRAPH</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_LINK</code></td>
 *              <td align="center"><span class="entypo-icon-link"></span></td>
 *              <td align="left"><code>ICON_LINK</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_LINKEDIN</code></td>
 *              <td align="center"><span class="entypo-icon-linkedin"></span></td>
 *              <td align="left"><code>ICON_LINKEDIN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_LINKEDIN_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-linkedin-with-circle"></span></td>
 *              <td align="left"><code>ICON_LINKEDIN_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_LIST</code></td>
 *              <td align="center"><span class="entypo-icon-list"></span></td>
 *              <td align="left"><code>ICON_LIST</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_LOCATION</code></td>
 *              <td align="center"><span class="entypo-icon-location"></span></td>
 *              <td align="left"><code>ICON_LOCATION</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_LOCATION_PIN</code></td>
 *              <td align="center"><span class="entypo-icon-location-pin"></span></td>
 *              <td align="left"><code>ICON_LOCATION_PIN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_LOCK</code></td>
 *              <td align="center"><span class="entypo-icon-lock"></span></td>
 *              <td align="left"><code>ICON_LOCK</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_LOCK_OPEN</code></td>
 *              <td align="center"><span class="entypo-icon-lock-open"></span></td>
 *              <td align="left"><code>ICON_LOCK_OPEN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_LOG_OUT</code></td>
 *              <td align="center"><span class="entypo-icon-log-out"></span></td>
 *              <td align="left"><code>ICON_LOG_OUT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_LOGIN</code></td>
 *              <td align="center"><span class="entypo-icon-login"></span></td>
 *              <td align="left"><code>ICON_LOGIN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_LOOP</code></td>
 *              <td align="center"><span class="entypo-icon-loop"></span></td>
 *              <td align="left"><code>ICON_LOOP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_MAGNET</code></td>
 *              <td align="center"><span class="entypo-icon-magnet"></span></td>
 *              <td align="left"><code>ICON_MAGNET</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_MAGNIFYING_GLASS</code></td>
 *              <td align="center"><span class="entypo-icon-magnifying-glass"></span></td>
 *              <td align="left"><code>ICON_MAGNIFYING_GLASS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_MAIL</code></td>
 *              <td align="center"><span class="entypo-icon-mail"></span></td>
 *              <td align="left"><code>ICON_MAIL</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_MAIL_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-mail-with-circle"></span></td>
 *              <td align="left"><code>ICON_MAIL_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_MAN</code></td>
 *              <td align="center"><span class="entypo-icon-man"></span></td>
 *              <td align="left"><code>ICON_MAN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_MAP</code></td>
 *              <td align="center"><span class="entypo-icon-map"></span></td>
 *              <td align="left"><code>ICON_MAP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_MASK</code></td>
 *              <td align="center"><span class="entypo-icon-mask"></span></td>
 *              <td align="left"><code>ICON_MASK</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_MEDAL</code></td>
 *              <td align="center"><span class="entypo-icon-medal"></span></td>
 *              <td align="left"><code>ICON_MEDAL</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_MEDIUM</code></td>
 *              <td align="center"><span class="entypo-icon-medium"></span></td>
 *              <td align="left"><code>ICON_MEDIUM</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_MEDIUM_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-medium-with-circle"></span></td>
 *              <td align="left"><code>ICON_MEDIUM_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_MEGAPHONE</code></td>
 *              <td align="center"><span class="entypo-icon-megaphone"></span></td>
 *              <td align="left"><code>ICON_MEGAPHONE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_MENU</code></td>
 *              <td align="center"><span class="entypo-icon-menu"></span></td>
 *              <td align="left"><code>ICON_MENU</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_MERGE</code></td>
 *              <td align="center"><span class="entypo-icon-merge"></span></td>
 *              <td align="left"><code>ICON_MERGE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_MESSAGE</code></td>
 *              <td align="center"><span class="entypo-icon-message"></span></td>
 *              <td align="left"><code>ICON_MESSAGE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_MIC</code></td>
 *              <td align="center"><span class="entypo-icon-mic"></span></td>
 *              <td align="left"><code>ICON_MIC</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_MINUS</code></td>
 *              <td align="center"><span class="entypo-icon-minus"></span></td>
 *              <td align="left"><code>ICON_MINUS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_MIXI</code></td>
 *              <td align="center"><span class="entypo-icon-mixi"></span></td>
 *              <td align="left"><code>ICON_MIXI</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_MOBILE</code></td>
 *              <td align="center"><span class="entypo-icon-mobile"></span></td>
 *              <td align="left"><code>ICON_MOBILE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_MODERN_MIC</code></td>
 *              <td align="center"><span class="entypo-icon-modern-mic"></span></td>
 *              <td align="left"><code>ICON_MODERN_MIC</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_MOON</code></td>
 *              <td align="center"><span class="entypo-icon-moon"></span></td>
 *              <td align="left"><code>ICON_MOON</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_MOUSE</code></td>
 *              <td align="center"><span class="entypo-icon-mouse"></span></td>
 *              <td align="left"><code>ICON_MOUSE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_MOUSE_POINTER</code></td>
 *              <td align="center"><span class="entypo-icon-mouse-pointer"></span></td>
 *              <td align="left"><code>ICON_MOUSE_POINTER</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_MUSIC</code></td>
 *              <td align="center"><span class="entypo-icon-music"></span></td>
 *              <td align="left"><code>ICON_MUSIC</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_NETWORK</code></td>
 *              <td align="center"><span class="entypo-icon-network"></span></td>
 *              <td align="left"><code>ICON_NETWORK</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_NEW</code></td>
 *              <td align="center"><span class="entypo-icon-new"></span></td>
 *              <td align="left"><code>ICON_NEW</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_NEW_MESSAGE</code></td>
 *              <td align="center"><span class="entypo-icon-new-message"></span></td>
 *              <td align="left"><code>ICON_NEW_MESSAGE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_NEWS</code></td>
 *              <td align="center"><span class="entypo-icon-news"></span></td>
 *              <td align="left"><code>ICON_NEWS</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_NEWSLETTER</code></td>
 *              <td align="center"><span class="entypo-icon-newsletter"></span></td>
 *              <td align="left"><code>ICON_NEWSLETTER</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_NOTE</code></td>
 *              <td align="center"><span class="entypo-icon-note"></span></td>
 *              <td align="left"><code>ICON_NOTE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_NOTIFICATION</code></td>
 *              <td align="center"><span class="entypo-icon-notification"></span></td>
 *              <td align="left"><code>ICON_NOTIFICATION</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_NOTIFICATIONS_OFF</code></td>
 *              <td align="center"><span class="entypo-icon-notifications-off"></span></td>
 *              <td align="left"><code>ICON_NOTIFICATIONS_OFF</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_OLD_MOBILE</code></td>
 *              <td align="center"><span class="entypo-icon-old-mobile"></span></td>
 *              <td align="left"><code>ICON_OLD_MOBILE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_OLD_PHONE</code></td>
 *              <td align="center"><span class="entypo-icon-old-phone"></span></td>
 *              <td align="left"><code>ICON_OLD_PHONE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ONEDRIVE</code></td>
 *              <td align="center"><span class="entypo-icon-onedrive"></span></td>
 *              <td align="left"><code>ICON_ONEDRIVE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_OPEN_BOOK</code></td>
 *              <td align="center"><span class="entypo-icon-open-book"></span></td>
 *              <td align="left"><code>ICON_OPEN_BOOK</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_PALETTE</code></td>
 *              <td align="center"><span class="entypo-icon-palette"></span></td>
 *              <td align="left"><code>ICON_PALETTE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_PAPER_PLANE</code></td>
 *              <td align="center"><span class="entypo-icon-paper-plane"></span></td>
 *              <td align="left"><code>ICON_PAPER_PLANE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_PAYPAL</code></td>
 *              <td align="center"><span class="entypo-icon-paypal"></span></td>
 *              <td align="left"><code>ICON_PAYPAL</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_PENCIL</code></td>
 *              <td align="center"><span class="entypo-icon-pencil"></span></td>
 *              <td align="left"><code>ICON_PENCIL</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_PHONE</code></td>
 *              <td align="center"><span class="entypo-icon-phone"></span></td>
 *              <td align="left"><code>ICON_PHONE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_PICASA</code></td>
 *              <td align="center"><span class="entypo-icon-picasa"></span></td>
 *              <td align="left"><code>ICON_PICASA</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_PIE_CHART</code></td>
 *              <td align="center"><span class="entypo-icon-pie-chart"></span></td>
 *              <td align="left"><code>ICON_PIE_CHART</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_PIN</code></td>
 *              <td align="center"><span class="entypo-icon-pin"></span></td>
 *              <td align="left"><code>ICON_PIN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_PINTEREST</code></td>
 *              <td align="center"><span class="entypo-icon-pinterest"></span></td>
 *              <td align="left"><code>ICON_PINTEREST</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_PINTEREST_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-pinterest-with-circle"></span></td>
 *              <td align="left"><code>ICON_PINTEREST_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_PLUS</code></td>
 *              <td align="center"><span class="entypo-icon-plus"></span></td>
 *              <td align="left"><code>ICON_PLUS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_POPUP</code></td>
 *              <td align="center"><span class="entypo-icon-popup"></span></td>
 *              <td align="left"><code>ICON_POPUP</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_POWER_PLUG</code></td>
 *              <td align="center"><span class="entypo-icon-power-plug"></span></td>
 *              <td align="left"><code>ICON_POWER_PLUG</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_PRICE_RIBBON</code></td>
 *              <td align="center"><span class="entypo-icon-price-ribbon"></span></td>
 *              <td align="left"><code>ICON_PRICE_RIBBON</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_PRICE_TAG</code></td>
 *              <td align="center"><span class="entypo-icon-price-tag"></span></td>
 *              <td align="left"><code>ICON_PRICE_TAG</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_PRINT</code></td>
 *              <td align="center"><span class="entypo-icon-print"></span></td>
 *              <td align="left"><code>ICON_PRINT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_PROGRESS_EMPTY</code></td>
 *              <td align="center"><span class="entypo-icon-progress-empty"></span></td>
 *              <td align="left"><code>ICON_PROGRESS_EMPTY</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_PROGRESS_FULL</code></td>
 *              <td align="center"><span class="entypo-icon-progress-full"></span></td>
 *              <td align="left"><code>ICON_PROGRESS_FULL</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_PROGRESS_ONE</code></td>
 *              <td align="center"><span class="entypo-icon-progress-one"></span></td>
 *              <td align="left"><code>ICON_PROGRESS_ONE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_PROGRESS_TWO</code></td>
 *              <td align="center"><span class="entypo-icon-progress-two"></span></td>
 *              <td align="left"><code>ICON_PROGRESS_TWO</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_PUBLISH</code></td>
 *              <td align="center"><span class="entypo-icon-publish"></span></td>
 *              <td align="left"><code>ICON_PUBLISH</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_QQ</code></td>
 *              <td align="center"><span class="entypo-icon-qq"></span></td>
 *              <td align="left"><code>ICON_QQ</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_QQ_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-qq-with-circle"></span></td>
 *              <td align="left"><code>ICON_QQ_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_QUOTE</code></td>
 *              <td align="center"><span class="entypo-icon-quote"></span></td>
 *              <td align="left"><code>ICON_QUOTE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_RADIO</code></td>
 *              <td align="center"><span class="entypo-icon-radio"></span></td>
 *              <td align="left"><code>ICON_RADIO</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_RAFT</code></td>
 *              <td align="center"><span class="entypo-icon-raft"></span></td>
 *              <td align="left"><code>ICON_RAFT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_RAFT_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-raft-with-circle"></span></td>
 *              <td align="left"><code>ICON_RAFT_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_RAINBOW</code></td>
 *              <td align="center"><span class="entypo-icon-rainbow"></span></td>
 *              <td align="left"><code>ICON_RAINBOW</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_RDIO</code></td>
 *              <td align="center"><span class="entypo-icon-rdio"></span></td>
 *              <td align="left"><code>ICON_RDIO</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_RDIO_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-rdio-with-circle"></span></td>
 *              <td align="left"><code>ICON_RDIO_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_REMOVE_USER</code></td>
 *              <td align="center"><span class="entypo-icon-remove-user"></span></td>
 *              <td align="left"><code>ICON_REMOVE_USER</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_RENREN</code></td>
 *              <td align="center"><span class="entypo-icon-renren"></span></td>
 *              <td align="left"><code>ICON_RENREN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_REPLY</code></td>
 *              <td align="center"><span class="entypo-icon-reply"></span></td>
 *              <td align="left"><code>ICON_REPLY</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_REPLY_ALL</code></td>
 *              <td align="center"><span class="entypo-icon-reply-all"></span></td>
 *              <td align="left"><code>ICON_REPLY_ALL</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_RESIZE_100_PERCENT</code></td>
 *              <td align="center"><span class="entypo-icon-resize-100-percent"></span></td>
 *              <td align="left"><code>ICON_RESIZE_100_PERCENT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_RESIZE_FULL_SCREEN</code></td>
 *              <td align="center"><span class="entypo-icon-resize-full-screen"></span></td>
 *              <td align="left"><code>ICON_RESIZE_FULL_SCREEN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_RETWEET</code></td>
 *              <td align="center"><span class="entypo-icon-retweet"></span></td>
 *              <td align="left"><code>ICON_RETWEET</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_ROCKET</code></td>
 *              <td align="center"><span class="entypo-icon-rocket"></span></td>
 *              <td align="left"><code>ICON_ROCKET</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_ROUND_BRUSH</code></td>
 *              <td align="center"><span class="entypo-icon-round-brush"></span></td>
 *              <td align="left"><code>ICON_ROUND_BRUSH</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_RSS</code></td>
 *              <td align="center"><span class="entypo-icon-rss"></span></td>
 *              <td align="left"><code>ICON_RSS</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_RULER</code></td>
 *              <td align="center"><span class="entypo-icon-ruler"></span></td>
 *              <td align="left"><code>ICON_RULER</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SAVE</code></td>
 *              <td align="center"><span class="entypo-icon-save"></span></td>
 *              <td align="left"><code>ICON_SAVE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SCISSORS</code></td>
 *              <td align="center"><span class="entypo-icon-scissors"></span></td>
 *              <td align="left"><code>ICON_SCISSORS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SCRIBD</code></td>
 *              <td align="center"><span class="entypo-icon-scribd"></span></td>
 *              <td align="left"><code>ICON_SCRIBD</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SELECT_ARROWS</code></td>
 *              <td align="center"><span class="entypo-icon-select-arrows"></span></td>
 *              <td align="left"><code>ICON_SELECT_ARROWS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SHARE</code></td>
 *              <td align="center"><span class="entypo-icon-share"></span></td>
 *              <td align="left"><code>ICON_SHARE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SHARE_ALTERNATIVE</code></td>
 *              <td align="center"><span class="entypo-icon-share-alternative"></span></td>
 *              <td align="left"><code>ICON_SHARE_ALTERNATIVE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SHAREABLE</code></td>
 *              <td align="center"><span class="entypo-icon-shareable"></span></td>
 *              <td align="left"><code>ICON_SHAREABLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SHIELD</code></td>
 *              <td align="center"><span class="entypo-icon-shield"></span></td>
 *              <td align="left"><code>ICON_SHIELD</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SHOP</code></td>
 *              <td align="center"><span class="entypo-icon-shop"></span></td>
 *              <td align="left"><code>ICON_SHOP</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SHOPPING_BAG</code></td>
 *              <td align="center"><span class="entypo-icon-shopping-bag"></span></td>
 *              <td align="left"><code>ICON_SHOPPING_BAG</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SHOPPING_BASKET</code></td>
 *              <td align="center"><span class="entypo-icon-shopping-basket"></span></td>
 *              <td align="left"><code>ICON_SHOPPING_BASKET</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SHOPPING_CART</code></td>
 *              <td align="center"><span class="entypo-icon-shopping-cart"></span></td>
 *              <td align="left"><code>ICON_SHOPPING_CART</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SHUFFLE</code></td>
 *              <td align="center"><span class="entypo-icon-shuffle"></span></td>
 *              <td align="left"><code>ICON_SHUFFLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SIGNAL</code></td>
 *              <td align="center"><span class="entypo-icon-signal"></span></td>
 *              <td align="left"><code>ICON_SIGNAL</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SINA_WEIBO</code></td>
 *              <td align="center"><span class="entypo-icon-sina-weibo"></span></td>
 *              <td align="left"><code>ICON_SINA_WEIBO</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SKYPE</code></td>
 *              <td align="center"><span class="entypo-icon-skype"></span></td>
 *              <td align="left"><code>ICON_SKYPE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SKYPE_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-skype-with-circle"></span></td>
 *              <td align="left"><code>ICON_SKYPE_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SLIDESHARE</code></td>
 *              <td align="center"><span class="entypo-icon-slideshare"></span></td>
 *              <td align="left"><code>ICON_SLIDESHARE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SMASHING</code></td>
 *              <td align="center"><span class="entypo-icon-smashing"></span></td>
 *              <td align="left"><code>ICON_SMASHING</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SOUND</code></td>
 *              <td align="center"><span class="entypo-icon-sound"></span></td>
 *              <td align="left"><code>ICON_SOUND</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SOUND_MIX</code></td>
 *              <td align="center"><span class="entypo-icon-sound-mix"></span></td>
 *              <td align="left"><code>ICON_SOUND_MIX</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SOUND_MUTE</code></td>
 *              <td align="center"><span class="entypo-icon-sound-mute"></span></td>
 *              <td align="left"><code>ICON_SOUND_MUTE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SOUNDCLOUD</code></td>
 *              <td align="center"><span class="entypo-icon-soundcloud"></span></td>
 *              <td align="left"><code>ICON_SOUNDCLOUD</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SPORTS_CLUB</code></td>
 *              <td align="center"><span class="entypo-icon-sports-club"></span></td>
 *              <td align="left"><code>ICON_SPORTS_CLUB</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SPOTIFY</code></td>
 *              <td align="center"><span class="entypo-icon-spotify"></span></td>
 *              <td align="left"><code>ICON_SPOTIFY</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SPOTIFY_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-spotify-with-circle"></span></td>
 *              <td align="left"><code>ICON_SPOTIFY_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SPREADSHEET</code></td>
 *              <td align="center"><span class="entypo-icon-spreadsheet"></span></td>
 *              <td align="left"><code>ICON_SPREADSHEET</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SQUARED_CROSS</code></td>
 *              <td align="center"><span class="entypo-icon-squared-cross"></span></td>
 *              <td align="left"><code>ICON_SQUARED_CROSS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SQUARED_MINUS</code></td>
 *              <td align="center"><span class="entypo-icon-squared-minus"></span></td>
 *              <td align="left"><code>ICON_SQUARED_MINUS</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SQUARED_PLUS</code></td>
 *              <td align="center"><span class="entypo-icon-squared-plus"></span></td>
 *              <td align="left"><code>ICON_SQUARED_PLUS</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_STAR</code></td>
 *              <td align="center"><span class="entypo-icon-star"></span></td>
 *              <td align="left"><code>ICON_STAR</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_STAR_OUTLINED</code></td>
 *              <td align="center"><span class="entypo-icon-star-outlined"></span></td>
 *              <td align="left"><code>ICON_STAR_OUTLINED</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_STOPWATCH</code></td>
 *              <td align="center"><span class="entypo-icon-stopwatch"></span></td>
 *              <td align="left"><code>ICON_STOPWATCH</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_STUMBLEUPON</code></td>
 *              <td align="center"><span class="entypo-icon-stumbleupon"></span></td>
 *              <td align="left"><code>ICON_STUMBLEUPON</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_STUMBLEUPON_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-stumbleupon-with-circle"></span></td>
 *              <td align="left"><code>ICON_STUMBLEUPON_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SUITCASE</code></td>
 *              <td align="center"><span class="entypo-icon-suitcase"></span></td>
 *              <td align="left"><code>ICON_SUITCASE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SWAP</code></td>
 *              <td align="center"><span class="entypo-icon-swap"></span></td>
 *              <td align="left"><code>ICON_SWAP</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SWARM</code></td>
 *              <td align="center"><span class="entypo-icon-swarm"></span></td>
 *              <td align="left"><code>ICON_SWARM</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_SWEDEN</code></td>
 *              <td align="center"><span class="entypo-icon-sweden"></span></td>
 *              <td align="left"><code>ICON_SWEDEN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_SWITCH</code></td>
 *              <td align="center"><span class="entypo-icon-switch"></span></td>
 *              <td align="left"><code>ICON_SWITCH</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_TABLET</code></td>
 *              <td align="center"><span class="entypo-icon-tablet"></span></td>
 *              <td align="left"><code>ICON_TABLET</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_TABLET_MOBILE_COMBO</code></td>
 *              <td align="center"><span class="entypo-icon-tablet-mobile-combo"></span></td>
 *              <td align="left"><code>ICON_TABLET_MOBILE_COMBO</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_TAG</code></td>
 *              <td align="center"><span class="entypo-icon-tag"></span></td>
 *              <td align="left"><code>ICON_TAG</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_TEXT</code></td>
 *              <td align="center"><span class="entypo-icon-text"></span></td>
 *              <td align="left"><code>ICON_TEXT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_TEXT_DOCUMENT</code></td>
 *              <td align="center"><span class="entypo-icon-text-document"></span></td>
 *              <td align="left"><code>ICON_TEXT_DOCUMENT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_TEXT_DOCUMENT_INVERTED</code></td>
 *              <td align="center"><span class="entypo-icon-text-document-inverted"></span></td>
 *              <td align="left"><code>ICON_TEXT_DOCUMENT_INVERTED</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_THERMOMETER</code></td>
 *              <td align="center"><span class="entypo-icon-thermometer"></span></td>
 *              <td align="left"><code>ICON_THERMOMETER</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_THUMBS_DOWN</code></td>
 *              <td align="center"><span class="entypo-icon-thumbs-down"></span></td>
 *              <td align="left"><code>ICON_THUMBS_DOWN</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_THUMBS_UP</code></td>
 *              <td align="center"><span class="entypo-icon-thumbs-up"></span></td>
 *              <td align="left"><code>ICON_THUMBS_UP</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_THUNDER_CLOUD</code></td>
 *              <td align="center"><span class="entypo-icon-thunder-cloud"></span></td>
 *              <td align="left"><code>ICON_THUNDER_CLOUD</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_TICKET</code></td>
 *              <td align="center"><span class="entypo-icon-ticket"></span></td>
 *              <td align="left"><code>ICON_TICKET</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_TIME_SLOT</code></td>
 *              <td align="center"><span class="entypo-icon-time-slot"></span></td>
 *              <td align="left"><code>ICON_TIME_SLOT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_TOOLS</code></td>
 *              <td align="center"><span class="entypo-icon-tools"></span></td>
 *              <td align="left"><code>ICON_TOOLS</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_TRAFFIC_CONE</code></td>
 *              <td align="center"><span class="entypo-icon-traffic-cone"></span></td>
 *              <td align="left"><code>ICON_TRAFFIC_CONE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_TRASH</code></td>
 *              <td align="center"><span class="entypo-icon-trash"></span></td>
 *              <td align="left"><code>ICON_TRASH</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_TREE</code></td>
 *              <td align="center"><span class="entypo-icon-tree"></span></td>
 *              <td align="left"><code>ICON_TREE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_TRIANGLE_DOWN</code></td>
 *              <td align="center"><span class="entypo-icon-triangle-down"></span></td>
 *              <td align="left"><code>ICON_TRIANGLE_DOWN</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_TRIANGLE_LEFT</code></td>
 *              <td align="center"><span class="entypo-icon-triangle-left"></span></td>
 *              <td align="left"><code>ICON_TRIANGLE_LEFT</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_TRIANGLE_RIGHT</code></td>
 *              <td align="center"><span class="entypo-icon-triangle-right"></span></td>
 *              <td align="left"><code>ICON_TRIANGLE_RIGHT</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_TRIANGLE_UP</code></td>
 *              <td align="center"><span class="entypo-icon-triangle-up"></span></td>
 *              <td align="left"><code>ICON_TRIANGLE_UP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_TRIPADVISOR</code></td>
 *              <td align="center"><span class="entypo-icon-tripadvisor"></span></td>
 *              <td align="left"><code>ICON_TRIPADVISOR</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_TROPHY</code></td>
 *              <td align="center"><span class="entypo-icon-trophy"></span></td>
 *              <td align="left"><code>ICON_TROPHY</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_TUMBLR</code></td>
 *              <td align="center"><span class="entypo-icon-tumblr"></span></td>
 *              <td align="left"><code>ICON_TUMBLR</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_TUMBLR_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-tumblr-with-circle"></span></td>
 *              <td align="left"><code>ICON_TUMBLR_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_TV</code></td>
 *              <td align="center"><span class="entypo-icon-tv"></span></td>
 *              <td align="left"><code>ICON_TV</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_TWITTER</code></td>
 *              <td align="center"><span class="entypo-icon-twitter"></span></td>
 *              <td align="left"><code>ICON_TWITTER</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_TWITTER_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-twitter-with-circle"></span></td>
 *              <td align="left"><code>ICON_TWITTER_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_TYPING</code></td>
 *              <td align="center"><span class="entypo-icon-typing"></span></td>
 *              <td align="left"><code>ICON_TYPING</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_UNINSTALL</code></td>
 *              <td align="center"><span class="entypo-icon-uninstall"></span></td>
 *              <td align="left"><code>ICON_UNINSTALL</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_UNREAD</code></td>
 *              <td align="center"><span class="entypo-icon-unread"></span></td>
 *              <td align="left"><code>ICON_UNREAD</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_UNTAG</code></td>
 *              <td align="center"><span class="entypo-icon-untag"></span></td>
 *              <td align="left"><code>ICON_UNTAG</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_UPLOAD</code></td>
 *              <td align="center"><span class="entypo-icon-upload"></span></td>
 *              <td align="left"><code>ICON_UPLOAD</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_UPLOAD_TO_CLOUD</code></td>
 *              <td align="center"><span class="entypo-icon-upload-to-cloud"></span></td>
 *              <td align="left"><code>ICON_UPLOAD_TO_CLOUD</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_USER</code></td>
 *              <td align="center"><span class="entypo-icon-user"></span></td>
 *              <td align="left"><code>ICON_USER</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_USERS</code></td>
 *              <td align="center"><span class="entypo-icon-users"></span></td>
 *              <td align="left"><code>ICON_USERS</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_V_CARD</code></td>
 *              <td align="center"><span class="entypo-icon-v-card"></span></td>
 *              <td align="left"><code>ICON_V_CARD</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_VIDEO</code></td>
 *              <td align="center"><span class="entypo-icon-video"></span></td>
 *              <td align="left"><code>ICON_VIDEO</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_VIDEO_CAMERA</code></td>
 *              <td align="center"><span class="entypo-icon-video-camera"></span></td>
 *              <td align="left"><code>ICON_VIDEO_CAMERA</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_VIMEO</code></td>
 *              <td align="center"><span class="entypo-icon-vimeo"></span></td>
 *              <td align="left"><code>ICON_VIMEO</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_VIMEO_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-vimeo-with-circle"></span></td>
 *              <td align="left"><code>ICON_VIMEO_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_VINE</code></td>
 *              <td align="center"><span class="entypo-icon-vine"></span></td>
 *              <td align="left"><code>ICON_VINE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_VINE_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-vine-with-circle"></span></td>
 *              <td align="left"><code>ICON_VINE_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_VINYL</code></td>
 *              <td align="center"><span class="entypo-icon-vinyl"></span></td>
 *              <td align="left"><code>ICON_VINYL</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_VK</code></td>
 *              <td align="center"><span class="entypo-icon-vk"></span></td>
 *              <td align="left"><code>ICON_VK</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_VK_ALTERNITIVE</code></td>
 *              <td align="center"><span class="entypo-icon-vk-alternitive"></span></td>
 *              <td align="left"><code>ICON_VK_ALTERNITIVE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_VK_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-vk-with-circle"></span></td>
 *              <td align="left"><code>ICON_VK_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_VOICEMAIL</code></td>
 *              <td align="center"><span class="entypo-icon-voicemail"></span></td>
 *              <td align="left"><code>ICON_VOICEMAIL</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_WALLET</code></td>
 *              <td align="center"><span class="entypo-icon-wallet"></span></td>
 *              <td align="left"><code>ICON_WALLET</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_WARNING</code></td>
 *              <td align="center"><span class="entypo-icon-warning"></span></td>
 *              <td align="left"><code>ICON_WARNING</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_WATER</code></td>
 *              <td align="center"><span class="entypo-icon-water"></span></td>
 *              <td align="left"><code>ICON_WATER</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_WINDOWS_STORE</code></td>
 *              <td align="center"><span class="entypo-icon-windows-store"></span></td>
 *              <td align="left"><code>ICON_WINDOWS_STORE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_XING</code></td>
 *              <td align="center"><span class="entypo-icon-xing"></span></td>
 *              <td align="left"><code>ICON_XING</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_XING_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-xing-with-circle"></span></td>
 *              <td align="left"><code>ICON_XING_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_YELP</code></td>
 *              <td align="center"><span class="entypo-icon-yelp"></span></td>
 *              <td align="left"><code>ICON_YELP</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_YOUKO</code></td>
 *              <td align="center"><span class="entypo-icon-youko"></span></td>
 *              <td align="left"><code>ICON_YOUKO</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_YOUKO_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-youko-with-circle"></span></td>
 *              <td align="left"><code>ICON_YOUKO_WITH_CIRCLE</code>
 *            </tr>
 *            <tr class="row-odd">
 *              <td align="right"><code>ENTYPO_ICON_YOUTUBE</code></td>
 *              <td align="center"><span class="entypo-icon-youtube"></span></td>
 *              <td align="left"><code>ICON_YOUTUBE</code>
 *            </tr>
 *            <tr class="row-even">
 *              <td align="right"><code>ENTYPO_ICON_YOUTUBE_WITH_CIRCLE</code></td>
 *              <td align="center"><span class="entypo-icon-youtube-with-circle"></span></td>
 *              <td align="left"><code>ICON_YOUTUBE_WITH_CIRCLE</code>
 *            </tr>
 *          </tbody>
 *        </table>
 *      </div><!-- wy-table-responsive -->
 *    </center>
 *
 * \endrst
 */

#pragma once

// prevent individual pages from being generated for all of these
#if !defined(DOXYGEN_SHOULD_SKIP_THIS)

#define ENTYPO_ICON_500PX                             0x0000F100
#define ENTYPO_ICON_500PX_WITH_CIRCLE                 0x0000F101
#define ENTYPO_ICON_ADD_TO_LIST                       0x0000F102
#define ENTYPO_ICON_ADD_USER                          0x0000F103
#define ENTYPO_ICON_ADDRESS                           0x0000F104
#define ENTYPO_ICON_ADJUST                            0x0000F105
#define ENTYPO_ICON_AIR                               0x0000F106
#define ENTYPO_ICON_AIRCRAFT                          0x0000F107
#define ENTYPO_ICON_AIRCRAFT_LANDING                  0x0000F108
#define ENTYPO_ICON_AIRCRAFT_TAKE_OFF                 0x0000F109
#define ENTYPO_ICON_ALIGN_BOTTOM                      0x0000F10A
#define ENTYPO_ICON_ALIGN_HORIZONTAL_MIDDLE           0x0000F10B
#define ENTYPO_ICON_ALIGN_LEFT                        0x0000F10C
#define ENTYPO_ICON_ALIGN_RIGHT                       0x0000F10D
#define ENTYPO_ICON_ALIGN_TOP                         0x0000F10E
#define ENTYPO_ICON_ALIGN_VERTICAL_MIDDLE             0x0000F10F
#define ENTYPO_ICON_APP_STORE                         0x0000F110
#define ENTYPO_ICON_ARCHIVE                           0x0000F111
#define ENTYPO_ICON_AREA_GRAPH                        0x0000F112
#define ENTYPO_ICON_ARROW_BOLD_DOWN                   0x0000F113
#define ENTYPO_ICON_ARROW_BOLD_LEFT                   0x0000F114
#define ENTYPO_ICON_ARROW_BOLD_RIGHT                  0x0000F115
#define ENTYPO_ICON_ARROW_BOLD_UP                     0x0000F116
#define ENTYPO_ICON_ARROW_DOWN                        0x0000F117
#define ENTYPO_ICON_ARROW_LEFT                        0x0000F118
#define ENTYPO_ICON_ARROW_LONG_DOWN                   0x0000F119
#define ENTYPO_ICON_ARROW_LONG_LEFT                   0x0000F11A
#define ENTYPO_ICON_ARROW_LONG_RIGHT                  0x0000F11B
#define ENTYPO_ICON_ARROW_LONG_UP                     0x0000F11C
#define ENTYPO_ICON_ARROW_RIGHT                       0x0000F11D
#define ENTYPO_ICON_ARROW_UP                          0x0000F11E
#define ENTYPO_ICON_ARROW_WITH_CIRCLE_DOWN            0x0000F11F
#define ENTYPO_ICON_ARROW_WITH_CIRCLE_LEFT            0x0000F120
#define ENTYPO_ICON_ARROW_WITH_CIRCLE_RIGHT           0x0000F121
#define ENTYPO_ICON_ARROW_WITH_CIRCLE_UP              0x0000F122
#define ENTYPO_ICON_ATTACHMENT                        0x0000F123
#define ENTYPO_ICON_AWARENESS_RIBBON                  0x0000F124
#define ENTYPO_ICON_BACK                              0x0000F125
#define ENTYPO_ICON_BACK_IN_TIME                      0x0000F126
#define ENTYPO_ICON_BAIDU                             0x0000F127
#define ENTYPO_ICON_BAR_GRAPH                         0x0000F128
#define ENTYPO_ICON_BASECAMP                          0x0000F129
#define ENTYPO_ICON_BATTERY                           0x0000F12A
#define ENTYPO_ICON_BEAMED_NOTE                       0x0000F12B
#define ENTYPO_ICON_BEHANCE                           0x0000F12C
#define ENTYPO_ICON_BELL                              0x0000F12D
#define ENTYPO_ICON_BLACKBOARD                        0x0000F12E
#define ENTYPO_ICON_BLOCK                             0x0000F12F
#define ENTYPO_ICON_BOOK                              0x0000F130
#define ENTYPO_ICON_BOOKMARK                          0x0000F131
#define ENTYPO_ICON_BOOKMARKS                         0x0000F132
#define ENTYPO_ICON_BOWL                              0x0000F133
#define ENTYPO_ICON_BOX                               0x0000F134
#define ENTYPO_ICON_BRIEFCASE                         0x0000F135
#define ENTYPO_ICON_BROWSER                           0x0000F136
#define ENTYPO_ICON_BRUSH                             0x0000F137
#define ENTYPO_ICON_BUCKET                            0x0000F138
#define ENTYPO_ICON_BUG                               0x0000F139
#define ENTYPO_ICON_CAKE                              0x0000F13A
#define ENTYPO_ICON_CALCULATOR                        0x0000F13B
#define ENTYPO_ICON_CALENDAR                          0x0000F13C
#define ENTYPO_ICON_CAMERA                            0x0000F13D
#define ENTYPO_ICON_CCW                               0x0000F13E
#define ENTYPO_ICON_CHAT                              0x0000F13F
#define ENTYPO_ICON_CHECK                             0x0000F140
#define ENTYPO_ICON_CHEVRON_DOWN                      0x0000F141
#define ENTYPO_ICON_CHEVRON_LEFT                      0x0000F142
#define ENTYPO_ICON_CHEVRON_RIGHT                     0x0000F143
#define ENTYPO_ICON_CHEVRON_SMALL_DOWN                0x0000F144
#define ENTYPO_ICON_CHEVRON_SMALL_LEFT                0x0000F145
#define ENTYPO_ICON_CHEVRON_SMALL_RIGHT               0x0000F146
#define ENTYPO_ICON_CHEVRON_SMALL_UP                  0x0000F147
#define ENTYPO_ICON_CHEVRON_THIN_DOWN                 0x0000F148
#define ENTYPO_ICON_CHEVRON_THIN_LEFT                 0x0000F149
#define ENTYPO_ICON_CHEVRON_THIN_RIGHT                0x0000F14A
#define ENTYPO_ICON_CHEVRON_THIN_UP                   0x0000F14B
#define ENTYPO_ICON_CHEVRON_UP                        0x0000F14C
#define ENTYPO_ICON_CHEVRON_WITH_CIRCLE_DOWN          0x0000F14D
#define ENTYPO_ICON_CHEVRON_WITH_CIRCLE_LEFT          0x0000F14E
#define ENTYPO_ICON_CHEVRON_WITH_CIRCLE_RIGHT         0x0000F14F
#define ENTYPO_ICON_CHEVRON_WITH_CIRCLE_UP            0x0000F150
#define ENTYPO_ICON_CIRCLE                            0x0000F151
#define ENTYPO_ICON_CIRCLE_WITH_CROSS                 0x0000F152
#define ENTYPO_ICON_CIRCLE_WITH_MINUS                 0x0000F153
#define ENTYPO_ICON_CIRCLE_WITH_PLUS                  0x0000F154
#define ENTYPO_ICON_CIRCULAR_GRAPH                    0x0000F155
#define ENTYPO_ICON_CLAPPERBOARD                      0x0000F156
#define ENTYPO_ICON_CLASSIC_COMPUTER                  0x0000F157
#define ENTYPO_ICON_CLIPBOARD                         0x0000F158
#define ENTYPO_ICON_CLOCK                             0x0000F159
#define ENTYPO_ICON_CLOUD                             0x0000F15A
#define ENTYPO_ICON_CODE                              0x0000F15B
#define ENTYPO_ICON_COG                               0x0000F15C
#define ENTYPO_ICON_COLOURS                           0x0000F15D
#define ENTYPO_ICON_COMPASS                           0x0000F15E
#define ENTYPO_ICON_CONTROLLER_FAST_BACKWARD          0x0000F15F
#define ENTYPO_ICON_CONTROLLER_FAST_FORWARD           0x0000F160
#define ENTYPO_ICON_CONTROLLER_JUMP_TO_START          0x0000F161
#define ENTYPO_ICON_CONTROLLER_NEXT                   0x0000F162
#define ENTYPO_ICON_CONTROLLER_PAUS                   0x0000F163
#define ENTYPO_ICON_CONTROLLER_PLAY                   0x0000F164
#define ENTYPO_ICON_CONTROLLER_RECORD                 0x0000F165
#define ENTYPO_ICON_CONTROLLER_STOP                   0x0000F166
#define ENTYPO_ICON_CONTROLLER_VOLUME                 0x0000F167
#define ENTYPO_ICON_COPY                              0x0000F168
#define ENTYPO_ICON_CREATIVE_CLOUD                    0x0000F169
#define ENTYPO_ICON_CREATIVE_COMMONS                  0x0000F16A
#define ENTYPO_ICON_CREATIVE_COMMONS_ATTRIBUTION      0x0000F16B
#define ENTYPO_ICON_CREATIVE_COMMONS_NODERIVS         0x0000F16C
#define ENTYPO_ICON_CREATIVE_COMMONS_NONCOMMERCIAL_EU 0x0000F16D
#define ENTYPO_ICON_CREATIVE_COMMONS_NONCOMMERCIAL_US 0x0000F16E
#define ENTYPO_ICON_CREATIVE_COMMONS_PUBLIC_DOMAIN    0x0000F16F
#define ENTYPO_ICON_CREATIVE_COMMONS_REMIX            0x0000F170
#define ENTYPO_ICON_CREATIVE_COMMONS_SHARE            0x0000F171
#define ENTYPO_ICON_CREATIVE_COMMONS_SHAREALIKE       0x0000F172
#define ENTYPO_ICON_CREDIT                            0x0000F173
#define ENTYPO_ICON_CREDIT_CARD                       0x0000F174
#define ENTYPO_ICON_CROP                              0x0000F175
#define ENTYPO_ICON_CROSS                             0x0000F176
#define ENTYPO_ICON_CUP                               0x0000F177
#define ENTYPO_ICON_CW                                0x0000F178
#define ENTYPO_ICON_CYCLE                             0x0000F179
#define ENTYPO_ICON_DATABASE                          0x0000F17A
#define ENTYPO_ICON_DIAL_PAD                          0x0000F17B
#define ENTYPO_ICON_DIRECTION                         0x0000F17C
#define ENTYPO_ICON_DOCUMENT                          0x0000F17D
#define ENTYPO_ICON_DOCUMENT_LANDSCAPE                0x0000F17E
#define ENTYPO_ICON_DOCUMENTS                         0x0000F17F
#define ENTYPO_ICON_DOT_SINGLE                        0x0000F180
#define ENTYPO_ICON_DOTS_THREE_HORIZONTAL             0x0000F181
#define ENTYPO_ICON_DOTS_THREE_VERTICAL               0x0000F182
#define ENTYPO_ICON_DOTS_TWO_HORIZONTAL               0x0000F183
#define ENTYPO_ICON_DOTS_TWO_VERTICAL                 0x0000F184
#define ENTYPO_ICON_DOWNLOAD                          0x0000F185
#define ENTYPO_ICON_DRIBBBLE                          0x0000F186
#define ENTYPO_ICON_DRIBBBLE_WITH_CIRCLE              0x0000F187
#define ENTYPO_ICON_DRINK                             0x0000F188
#define ENTYPO_ICON_DRIVE                             0x0000F189
#define ENTYPO_ICON_DROP                              0x0000F18A
#define ENTYPO_ICON_DROPBOX                           0x0000F18B
#define ENTYPO_ICON_EDIT                              0x0000F18C
#define ENTYPO_ICON_EMAIL                             0x0000F18D
#define ENTYPO_ICON_EMOJI_FLIRT                       0x0000F18E
#define ENTYPO_ICON_EMOJI_HAPPY                       0x0000F18F
#define ENTYPO_ICON_EMOJI_NEUTRAL                     0x0000F190
#define ENTYPO_ICON_EMOJI_SAD                         0x0000F191
#define ENTYPO_ICON_ERASE                             0x0000F192
#define ENTYPO_ICON_ERASER                            0x0000F193
#define ENTYPO_ICON_EVERNOTE                          0x0000F194
#define ENTYPO_ICON_EXPORT                            0x0000F195
#define ENTYPO_ICON_EYE                               0x0000F196
#define ENTYPO_ICON_EYE_WITH_LINE                     0x0000F197
#define ENTYPO_ICON_FACEBOOK                          0x0000F198
#define ENTYPO_ICON_FACEBOOK_WITH_CIRCLE              0x0000F199
#define ENTYPO_ICON_FEATHER                           0x0000F19A
#define ENTYPO_ICON_FINGERPRINT                       0x0000F19B
#define ENTYPO_ICON_FLAG                              0x0000F19C
#define ENTYPO_ICON_FLASH                             0x0000F19D
#define ENTYPO_ICON_FLASHLIGHT                        0x0000F19E
#define ENTYPO_ICON_FLAT_BRUSH                        0x0000F19F
#define ENTYPO_ICON_FLATTR                            0x0000F1A0
#define ENTYPO_ICON_FLICKR                            0x0000F1A1
#define ENTYPO_ICON_FLICKR_WITH_CIRCLE                0x0000F1A2
#define ENTYPO_ICON_FLOW_BRANCH                       0x0000F1A3
#define ENTYPO_ICON_FLOW_CASCADE                      0x0000F1A4
#define ENTYPO_ICON_FLOW_LINE                         0x0000F1A5
#define ENTYPO_ICON_FLOW_PARALLEL                     0x0000F1A6
#define ENTYPO_ICON_FLOW_TREE                         0x0000F1A7
#define ENTYPO_ICON_FLOWER                            0x0000F1A8
#define ENTYPO_ICON_FOLDER                            0x0000F1A9
#define ENTYPO_ICON_FOLDER_IMAGES                     0x0000F1AA
#define ENTYPO_ICON_FOLDER_MUSIC                      0x0000F1AB
#define ENTYPO_ICON_FOLDER_VIDEO                      0x0000F1AC
#define ENTYPO_ICON_FORWARD                           0x0000F1AD
#define ENTYPO_ICON_FOURSQUARE                        0x0000F1AE
#define ENTYPO_ICON_FUNNEL                            0x0000F1AF
#define ENTYPO_ICON_GAME_CONTROLLER                   0x0000F1B0
#define ENTYPO_ICON_GAUGE                             0x0000F1B1
#define ENTYPO_ICON_GITHUB                            0x0000F1B2
#define ENTYPO_ICON_GITHUB_WITH_CIRCLE                0x0000F1B3
#define ENTYPO_ICON_GLOBE                             0x0000F1B4
#define ENTYPO_ICON_GOOGLE_DRIVE                      0x0000F1B5
#define ENTYPO_ICON_GOOGLE_HANGOUTS                   0x0000F1B6
#define ENTYPO_ICON_GOOGLE_PLAY                       0x0000F1B7
#define ENTYPO_ICON_GOOGLE_PLUS                       0x0000F1B8
#define ENTYPO_ICON_GOOGLE_PLUS_WITH_CIRCLE           0x0000F1B9
#define ENTYPO_ICON_GRADUATION_CAP                    0x0000F1BA
#define ENTYPO_ICON_GRID                              0x0000F1BB
#define ENTYPO_ICON_GROOVESHARK                       0x0000F1BC
#define ENTYPO_ICON_HAIR_CROSS                        0x0000F1BD
#define ENTYPO_ICON_HAND                              0x0000F1BE
#define ENTYPO_ICON_HEART                             0x0000F1BF
#define ENTYPO_ICON_HEART_OUTLINED                    0x0000F1C0
#define ENTYPO_ICON_HELP                              0x0000F1C1
#define ENTYPO_ICON_HELP_WITH_CIRCLE                  0x0000F1C2
#define ENTYPO_ICON_HOME                              0x0000F1C3
#define ENTYPO_ICON_HOUR_GLASS                        0x0000F1C4
#define ENTYPO_ICON_HOUZZ                             0x0000F1C5
#define ENTYPO_ICON_ICLOUD                            0x0000F1C6
#define ENTYPO_ICON_IMAGE                             0x0000F1C7
#define ENTYPO_ICON_IMAGE_INVERTED                    0x0000F1C8
#define ENTYPO_ICON_IMAGES                            0x0000F1C9
#define ENTYPO_ICON_INBOX                             0x0000F1CA
#define ENTYPO_ICON_INFINITY                          0x0000F1CB
#define ENTYPO_ICON_INFO                              0x0000F1CC
#define ENTYPO_ICON_INFO_WITH_CIRCLE                  0x0000F1CD
#define ENTYPO_ICON_INSTAGRAM                         0x0000F1CE
#define ENTYPO_ICON_INSTAGRAM_WITH_CIRCLE             0x0000F1CF
#define ENTYPO_ICON_INSTALL                           0x0000F1D0
#define ENTYPO_ICON_KEY                               0x0000F1D1
#define ENTYPO_ICON_KEYBOARD                          0x0000F1D2
#define ENTYPO_ICON_LAB_FLASK                         0x0000F1D3
#define ENTYPO_ICON_LANDLINE                          0x0000F1D4
#define ENTYPO_ICON_LANGUAGE                          0x0000F1D5
#define ENTYPO_ICON_LAPTOP                            0x0000F1D6
#define ENTYPO_ICON_LASTFM                            0x0000F1D7
#define ENTYPO_ICON_LASTFM_WITH_CIRCLE                0x0000F1D8
#define ENTYPO_ICON_LAYERS                            0x0000F1D9
#define ENTYPO_ICON_LEAF                              0x0000F1DA
#define ENTYPO_ICON_LEVEL_DOWN                        0x0000F1DB
#define ENTYPO_ICON_LEVEL_UP                          0x0000F1DC
#define ENTYPO_ICON_LIFEBUOY                          0x0000F1DD
#define ENTYPO_ICON_LIGHT_BULB                        0x0000F1DE
#define ENTYPO_ICON_LIGHT_DOWN                        0x0000F1DF
#define ENTYPO_ICON_LIGHT_UP                          0x0000F1E0
#define ENTYPO_ICON_LINE_GRAPH                        0x0000F1E1
#define ENTYPO_ICON_LINK                              0x0000F1E2
#define ENTYPO_ICON_LINKEDIN                          0x0000F1E3
#define ENTYPO_ICON_LINKEDIN_WITH_CIRCLE              0x0000F1E4
#define ENTYPO_ICON_LIST                              0x0000F1E5
#define ENTYPO_ICON_LOCATION                          0x0000F1E6
#define ENTYPO_ICON_LOCATION_PIN                      0x0000F1E7
#define ENTYPO_ICON_LOCK                              0x0000F1E8
#define ENTYPO_ICON_LOCK_OPEN                         0x0000F1E9
#define ENTYPO_ICON_LOG_OUT                           0x0000F1EA
#define ENTYPO_ICON_LOGIN                             0x0000F1EB
#define ENTYPO_ICON_LOOP                              0x0000F1EC
#define ENTYPO_ICON_MAGNET                            0x0000F1ED
#define ENTYPO_ICON_MAGNIFYING_GLASS                  0x0000F1EE
#define ENTYPO_ICON_MAIL                              0x0000F1EF
#define ENTYPO_ICON_MAIL_WITH_CIRCLE                  0x0000F1F0
#define ENTYPO_ICON_MAN                               0x0000F1F1
#define ENTYPO_ICON_MAP                               0x0000F1F2
#define ENTYPO_ICON_MASK                              0x0000F1F3
#define ENTYPO_ICON_MEDAL                             0x0000F1F4
#define ENTYPO_ICON_MEDIUM                            0x0000F1F5
#define ENTYPO_ICON_MEDIUM_WITH_CIRCLE                0x0000F1F6
#define ENTYPO_ICON_MEGAPHONE                         0x0000F1F7
#define ENTYPO_ICON_MENU                              0x0000F1F8
#define ENTYPO_ICON_MERGE                             0x0000F1F9
#define ENTYPO_ICON_MESSAGE                           0x0000F1FA
#define ENTYPO_ICON_MIC                               0x0000F1FB
#define ENTYPO_ICON_MINUS                             0x0000F1FC
#define ENTYPO_ICON_MIXI                              0x0000F1FD
#define ENTYPO_ICON_MOBILE                            0x0000F1FE
#define ENTYPO_ICON_MODERN_MIC                        0x0000F1FF
#define ENTYPO_ICON_MOON                              0x0000F200
#define ENTYPO_ICON_MOUSE                             0x0000F201
#define ENTYPO_ICON_MOUSE_POINTER                     0x0000F202
#define ENTYPO_ICON_MUSIC                             0x0000F203
#define ENTYPO_ICON_NETWORK                           0x0000F204
#define ENTYPO_ICON_NEW                               0x0000F205
#define ENTYPO_ICON_NEW_MESSAGE                       0x0000F206
#define ENTYPO_ICON_NEWS                              0x0000F207
#define ENTYPO_ICON_NEWSLETTER                        0x0000F208
#define ENTYPO_ICON_NOTE                              0x0000F209
#define ENTYPO_ICON_NOTIFICATION                      0x0000F20A
#define ENTYPO_ICON_NOTIFICATIONS_OFF                 0x0000F20B
#define ENTYPO_ICON_OLD_MOBILE                        0x0000F20C
#define ENTYPO_ICON_OLD_PHONE                         0x0000F20D
#define ENTYPO_ICON_ONEDRIVE                          0x0000F20E
#define ENTYPO_ICON_OPEN_BOOK                         0x0000F20F
#define ENTYPO_ICON_PALETTE                           0x0000F210
#define ENTYPO_ICON_PAPER_PLANE                       0x0000F211
#define ENTYPO_ICON_PAYPAL                            0x0000F212
#define ENTYPO_ICON_PENCIL                            0x0000F213
#define ENTYPO_ICON_PHONE                             0x0000F214
#define ENTYPO_ICON_PICASA                            0x0000F215
#define ENTYPO_ICON_PIE_CHART                         0x0000F216
#define ENTYPO_ICON_PIN                               0x0000F217
#define ENTYPO_ICON_PINTEREST                         0x0000F218
#define ENTYPO_ICON_PINTEREST_WITH_CIRCLE             0x0000F219
#define ENTYPO_ICON_PLUS                              0x0000F21A
#define ENTYPO_ICON_POPUP                             0x0000F21B
#define ENTYPO_ICON_POWER_PLUG                        0x0000F21C
#define ENTYPO_ICON_PRICE_RIBBON                      0x0000F21D
#define ENTYPO_ICON_PRICE_TAG                         0x0000F21E
#define ENTYPO_ICON_PRINT                             0x0000F21F
#define ENTYPO_ICON_PROGRESS_EMPTY                    0x0000F220
#define ENTYPO_ICON_PROGRESS_FULL                     0x0000F221
#define ENTYPO_ICON_PROGRESS_ONE                      0x0000F222
#define ENTYPO_ICON_PROGRESS_TWO                      0x0000F223
#define ENTYPO_ICON_PUBLISH                           0x0000F224
#define ENTYPO_ICON_QQ                                0x0000F225
#define ENTYPO_ICON_QQ_WITH_CIRCLE                    0x0000F226
#define ENTYPO_ICON_QUOTE                             0x0000F227
#define ENTYPO_ICON_RADIO                             0x0000F228
#define ENTYPO_ICON_RAFT                              0x0000F229
#define ENTYPO_ICON_RAFT_WITH_CIRCLE                  0x0000F22A
#define ENTYPO_ICON_RAINBOW                           0x0000F22B
#define ENTYPO_ICON_RDIO                              0x0000F22C
#define ENTYPO_ICON_RDIO_WITH_CIRCLE                  0x0000F22D
#define ENTYPO_ICON_REMOVE_USER                       0x0000F22E
#define ENTYPO_ICON_RENREN                            0x0000F22F
#define ENTYPO_ICON_REPLY                             0x0000F230
#define ENTYPO_ICON_REPLY_ALL                         0x0000F231
#define ENTYPO_ICON_RESIZE_100_PERCENT                0x0000F232
#define ENTYPO_ICON_RESIZE_FULL_SCREEN                0x0000F233
#define ENTYPO_ICON_RETWEET                           0x0000F234
#define ENTYPO_ICON_ROCKET                            0x0000F235
#define ENTYPO_ICON_ROUND_BRUSH                       0x0000F236
#define ENTYPO_ICON_RSS                               0x0000F237
#define ENTYPO_ICON_RULER                             0x0000F238
#define ENTYPO_ICON_SAVE                              0x0000F239
#define ENTYPO_ICON_SCISSORS                          0x0000F23A
#define ENTYPO_ICON_SCRIBD                            0x0000F23B
#define ENTYPO_ICON_SELECT_ARROWS                     0x0000F23C
#define ENTYPO_ICON_SHARE                             0x0000F23D
#define ENTYPO_ICON_SHARE_ALTERNATIVE                 0x0000F23E
#define ENTYPO_ICON_SHAREABLE                         0x0000F23F
#define ENTYPO_ICON_SHIELD                            0x0000F240
#define ENTYPO_ICON_SHOP                              0x0000F241
#define ENTYPO_ICON_SHOPPING_BAG                      0x0000F242
#define ENTYPO_ICON_SHOPPING_BASKET                   0x0000F243
#define ENTYPO_ICON_SHOPPING_CART                     0x0000F244
#define ENTYPO_ICON_SHUFFLE                           0x0000F245
#define ENTYPO_ICON_SIGNAL                            0x0000F246
#define ENTYPO_ICON_SINA_WEIBO                        0x0000F247
#define ENTYPO_ICON_SKYPE                             0x0000F248
#define ENTYPO_ICON_SKYPE_WITH_CIRCLE                 0x0000F249
#define ENTYPO_ICON_SLIDESHARE                        0x0000F24A
#define ENTYPO_ICON_SMASHING                          0x0000F24B
#define ENTYPO_ICON_SOUND                             0x0000F24C
#define ENTYPO_ICON_SOUND_MIX                         0x0000F24D
#define ENTYPO_ICON_SOUND_MUTE                        0x0000F24E
#define ENTYPO_ICON_SOUNDCLOUD                        0x0000F24F
#define ENTYPO_ICON_SPORTS_CLUB                       0x0000F250
#define ENTYPO_ICON_SPOTIFY                           0x0000F251
#define ENTYPO_ICON_SPOTIFY_WITH_CIRCLE               0x0000F252
#define ENTYPO_ICON_SPREADSHEET                       0x0000F253
#define ENTYPO_ICON_SQUARED_CROSS                     0x0000F254
#define ENTYPO_ICON_SQUARED_MINUS                     0x0000F255
#define ENTYPO_ICON_SQUARED_PLUS                      0x0000F256
#define ENTYPO_ICON_STAR                              0x0000F257
#define ENTYPO_ICON_STAR_OUTLINED                     0x0000F258
#define ENTYPO_ICON_STOPWATCH                         0x0000F259
#define ENTYPO_ICON_STUMBLEUPON                       0x0000F25A
#define ENTYPO_ICON_STUMBLEUPON_WITH_CIRCLE           0x0000F25B
#define ENTYPO_ICON_SUITCASE                          0x0000F25C
#define ENTYPO_ICON_SWAP                              0x0000F25D
#define ENTYPO_ICON_SWARM                             0x0000F25E
#define ENTYPO_ICON_SWEDEN                            0x0000F25F
#define ENTYPO_ICON_SWITCH                            0x0000F260
#define ENTYPO_ICON_TABLET                            0x0000F261
#define ENTYPO_ICON_TABLET_MOBILE_COMBO               0x0000F262
#define ENTYPO_ICON_TAG                               0x0000F263
#define ENTYPO_ICON_TEXT                              0x0000F264
#define ENTYPO_ICON_TEXT_DOCUMENT                     0x0000F265
#define ENTYPO_ICON_TEXT_DOCUMENT_INVERTED            0x0000F266
#define ENTYPO_ICON_THERMOMETER                       0x0000F267
#define ENTYPO_ICON_THUMBS_DOWN                       0x0000F268
#define ENTYPO_ICON_THUMBS_UP                         0x0000F269
#define ENTYPO_ICON_THUNDER_CLOUD                     0x0000F26A
#define ENTYPO_ICON_TICKET                            0x0000F26B
#define ENTYPO_ICON_TIME_SLOT                         0x0000F26C
#define ENTYPO_ICON_TOOLS                             0x0000F26D
#define ENTYPO_ICON_TRAFFIC_CONE                      0x0000F26E
#define ENTYPO_ICON_TRASH                             0x0000F26F
#define ENTYPO_ICON_TREE                              0x0000F270
#define ENTYPO_ICON_TRIANGLE_DOWN                     0x0000F271
#define ENTYPO_ICON_TRIANGLE_LEFT                     0x0000F272
#define ENTYPO_ICON_TRIANGLE_RIGHT                    0x0000F273
#define ENTYPO_ICON_TRIANGLE_UP                       0x0000F274
#define ENTYPO_ICON_TRIPADVISOR                       0x0000F275
#define ENTYPO_ICON_TROPHY                            0x0000F276
#define ENTYPO_ICON_TUMBLR                            0x0000F277
#define ENTYPO_ICON_TUMBLR_WITH_CIRCLE                0x0000F278
#define ENTYPO_ICON_TV                                0x0000F279
#define ENTYPO_ICON_TWITTER                           0x0000F27A
#define ENTYPO_ICON_TWITTER_WITH_CIRCLE               0x0000F27B
#define ENTYPO_ICON_TYPING                            0x0000F27C
#define ENTYPO_ICON_UNINSTALL                         0x0000F27D
#define ENTYPO_ICON_UNREAD                            0x0000F27E
#define ENTYPO_ICON_UNTAG                             0x0000F27F
#define ENTYPO_ICON_UPLOAD                            0x0000F280
#define ENTYPO_ICON_UPLOAD_TO_CLOUD                   0x0000F281
#define ENTYPO_ICON_USER                              0x0000F282
#define ENTYPO_ICON_USERS                             0x0000F283
#define ENTYPO_ICON_V_CARD                            0x0000F284
#define ENTYPO_ICON_VIDEO                             0x0000F285
#define ENTYPO_ICON_VIDEO_CAMERA                      0x0000F286
#define ENTYPO_ICON_VIMEO                             0x0000F287
#define ENTYPO_ICON_VIMEO_WITH_CIRCLE                 0x0000F288
#define ENTYPO_ICON_VINE                              0x0000F289
#define ENTYPO_ICON_VINE_WITH_CIRCLE                  0x0000F28A
#define ENTYPO_ICON_VINYL                             0x0000F28B
#define ENTYPO_ICON_VK                                0x0000F28C
#define ENTYPO_ICON_VK_ALTERNITIVE                    0x0000F28D
#define ENTYPO_ICON_VK_WITH_CIRCLE                    0x0000F28E
#define ENTYPO_ICON_VOICEMAIL                         0x0000F28F
#define ENTYPO_ICON_WALLET                            0x0000F290
#define ENTYPO_ICON_WARNING                           0x0000F291
#define ENTYPO_ICON_WATER                             0x0000F292
#define ENTYPO_ICON_WINDOWS_STORE                     0x0000F293
#define ENTYPO_ICON_XING                              0x0000F294
#define ENTYPO_ICON_XING_WITH_CIRCLE                  0x0000F295
#define ENTYPO_ICON_YELP                              0x0000F296
#define ENTYPO_ICON_YOUKO                             0x0000F297
#define ENTYPO_ICON_YOUKO_WITH_CIRCLE                 0x0000F298
#define ENTYPO_ICON_YOUTUBE                           0x0000F299
#define ENTYPO_ICON_YOUTUBE_WITH_CIRCLE               0x0000F29A

#endif // DOXYGEN_SHOULD_SKIP_THIS
