#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 6.3.9600.16384
//
//
// Buffer Definitions:
//
// cbuffer PS_CONSTANTS
// {
//
//   float4x4 scissorMat;               // Offset:    0 Size:    64
//   float4 scissorExt;                 // Offset:   64 Size:    16
//   float4 scissorScale;               // Offset:   80 Size:    16
//   float4x4 paintMat;                 // Offset:   96 Size:    64
//   float4 extent;                     // Offset:  160 Size:    16
//   float4 radius;                     // Offset:  176 Size:    16
//   float4 feather;                    // Offset:  192 Size:    16
//   float4 innerCol;                   // Offset:  208 Size:    16
//   float4 outerCol;                   // Offset:  224 Size:    16
//   float4 strokeMult;                 // Offset:  240 Size:    16
//   int texType;                       // Offset:  256 Size:     4
//   int type;                          // Offset:  260 Size:     4
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim Slot Elements
// ------------------------------ ---------- ------- ----------- ---- --------
// g_sampler                         sampler      NA          NA    0        1
// g_texture                         texture  float4          2d    0        1
// PS_CONSTANTS                      cbuffer      NA          NA    0        1
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Position              0   xyzw        0      POS   float
// TEXCOORD                 0   xy          1     NONE   float   xy
// TEXCOORD                 1     zw        1     NONE   float     zw
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
//
//
// Constant buffer to DX9 shader constant mappings:
//
// Target Reg Buffer  Start Reg # of Regs        Data Conversion
// ---------- ------- --------- --------- ----------------------
// c0         cb0             0         3  ( FLT, FLT, FLT, FLT)
// c3         cb0             4         5  ( FLT, FLT, FLT, FLT)
// c8         cb0            10         6  ( FLT, FLT, FLT, FLT)
// c14        cb0            16         1  ( INT, INT, FLT, FLT)
//
//
// Sampler/Resource to DX9 shader sampler mappings:
//
// Target Sampler Source Sampler  Source Resource
// -------------- --------------- ----------------
// s0             s0              t0
//
//
// Level9 shader bytecode:
//
    ps_2_x
    def c15, 0.5, 0, 0, 0
    def c16, 2, -1, 1, -0
    dcl t0
    dcl_2d s0
    mad r0.w, t0.x, c16.x, c16.y
    abs r0.x, r0.w
    add r0.x, -r0.x, c16.z
    mul r0.x, r0.x, c13.x
    min r1.w, r0.x, c16.z
    min r0.x, t0.y, c16.z
    mad r0.y, r1.w, r0.x, -c13.y
    mul r0.x, r0.x, r1.w
    cmp r1, r0.y, c16.w, c16.y
    texkill r1
    mul r0.yz, t0.z, c1.xxyw
    mad r0.yz, c0.xxyw, t0.w, r0
    add r0.yz, r0, c2.xxyw
    abs r0.yz, r0
    add r0.yz, r0, -c3.xxyw
    mov r0.w, c15.x
    mad_sat r0.yz, r0, -c4.xxyw, r0.w
    mul r0.y, r0.z, r0.y
    mul r0.z, c14.y, c14.y
    mul r1.xy, t0.z, c6
    mad r1.xy, c5, t0.w, r1
    add r1.xy, r1, c7
    mov r2.xy, c8
    add r1.zw, r2.xyxy, -c9.x
    abs r2.xy, r1
    add r1.zw, -r1, r2.xyxy
    max r2.xy, r1.zwzw, -c16.w
    dp2add r2.x, r2, r2, -c16.w
    rsq r2.x, r2.x
    rcp r2.x, r2.x
    max r2.y, r1.z, r1.w
    min r1.z, r2.y, -c16.w
    add r1.z, r2.x, r1.z
    add r1.z, r1.z, -c9.x
    mad r0.w, c10.x, r0.w, r1.z
    rcp r1.z, c10.x
    mul_sat r0.w, r0.w, r1.z
    mul r0.x, r0.y, r0.x
    rcp r2.x, c8.x
    rcp r2.y, c8.y
    mul r1.xy, r1, r2
    mov r2, c11
    add r2, -r2, c12
    mad r2, r0.w, r2, c11
    mul r2, r0.x, r2
    mov r3.xz, c16
    add r3, -r3.zzxx, c14.yxxy
    mul r3, r3, r3
    texld r4, t0, s0
    texld r1, r1, s0
    mul r5.xyz, r1.w, r1
    cmp r1.xyz, -r3.y, r5, r1
    cmp r1.yzw, -r3.z, r1.x, r1
    mul r1, r1, c11
    mul r1, r0.x, r1
    mul r5.xyz, r4.w, r4
    cmp r4.xyz, -r3.y, r5, r4
    cmp r4.yzw, -r3.z, r4.x, r4
    mul r4, r0.y, r4
    mul r4, r4, c11
    cmp r4, -r3.w, c16.z, r4
    cmp r1, -r3.x, r1, r4
    cmp r0, -r0.z, r2, r1
    mov oC0, r0

// approximately 65 instruction slots used (2 texture, 63 arithmetic)
ps_4_0
dcl_constantbuffer cb0[17], immediateIndexed
dcl_sampler s0, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_input_ps linear v1.xy
dcl_input_ps linear v1.zw
dcl_output o0.xyzw
dcl_temps 3
mul r0.xy, v1.wwww, cb0[1].xyxx
mad r0.xy, cb0[0].xyxx, v1.zzzz, r0.xyxx
add r0.xy, r0.xyxx, cb0[2].xyxx
add r0.xy, |r0.xyxx|, -cb0[4].xyxx
mad_sat r0.xy, -r0.xyxx, cb0[5].xyxx, l(0.500000, 0.500000, 0.000000, 0.000000)
mul r0.x, r0.y, r0.x
mad r0.y, v1.x, l(2.000000), l(-1.000000)
add r0.y, -|r0.y|, l(1.000000)
mul r0.y, r0.y, cb0[15].x
min r0.y, r0.y, l(1.000000)
min r0.z, v1.y, l(1.000000)
mul r0.y, r0.z, r0.y
if_z cb0[16].y
  mul r0.zw, v1.wwww, cb0[7].xxxy
  mad r0.zw, cb0[6].xxxy, v1.zzzz, r0.zzzw
  add r0.zw, r0.zzzw, cb0[8].xxxy
  add r1.xy, cb0[10].xyxx, -cb0[11].xxxx
  add r0.zw, |r0.zzzw|, -r1.xxxy
  max r1.x, r0.w, r0.z
  min r1.x, r1.x, l(0.000000)
  max r0.zw, r0.zzzw, l(0.000000, 0.000000, 0.000000, 0.000000)
  dp2 r0.z, r0.zwzz, r0.zwzz
  sqrt r0.z, r0.z
  add r0.z, r0.z, r1.x
  add r0.z, r0.z, -cb0[11].x
  mad r0.z, cb0[12].x, l(0.500000), r0.z
  div_sat r0.z, r0.z, cb0[12].x
  add r1.xyzw, -cb0[13].xyzw, cb0[14].xyzw
  mad r1.xyzw, r0.zzzz, r1.xyzw, cb0[13].xyzw
  mul r0.z, r0.x, r0.y
  mul o0.xyzw, r0.zzzz, r1.xyzw
else
  ieq r0.z, cb0[16].y, l(1)
  if_nz r0.z
    mul r0.zw, v1.wwww, cb0[7].xxxy
    mad r0.zw, cb0[6].xxxy, v1.zzzz, r0.zzzw
    add r0.zw, r0.zzzw, cb0[8].xxxy
    div r0.zw, r0.zzzw, cb0[10].xxxy
    sample r1.xyzw, r0.zwzz, t0.xyzw, s0
    ieq r0.zw, cb0[16].xxxx, l(0, 0, 1, 2)
    mul r2.xyz, r1.wwww, r1.xyzx
    movc r1.xyz, r0.zzzz, r2.xyzx, r1.xyzx
    movc r1.yzw, r0.wwww, r1.xxxx, r1.yyzw
    mul r1.xyzw, r1.xyzw, cb0[13].xyzw
    mul r0.z, r0.x, r0.y
    mul o0.xyzw, r0.zzzz, r1.xyzw
  else
    ieq r0.z, cb0[16].y, l(2)
    if_nz r0.z
      mov o0.xyzw, l(1.000000,1.000000,1.000000,1.000000)
    else
      sample r1.xyzw, v1.xyxx, t0.xyzw, s0
      ieq r0.zw, cb0[16].xxxx, l(0, 0, 1, 2)
      mul r2.xyz, r1.wwww, r1.xyzx
      movc r1.xyz, r0.zzzz, r2.xyzx, r1.xyzx
      movc r1.yzw, r0.wwww, r1.xxxx, r1.yyzw
      mul r1.xyzw, r0.xxxx, r1.xyzw
      mul o0.xyzw, r1.xyzw, cb0[13].xyzw
    endif
  endif
endif
lt r0.x, r0.y, cb0[15].y
discard_nz r0.x
ret
// Approximately 64 instruction slots used
#endif

const BYTE g_D3D11PixelShaderAA_Main[] =
{
     68,  88,  66,  67, 233, 140,
    133, 126,  48, 207,  72,  14,
    112,  26, 238, 211,  43,  10,
     86, 117,   1,   0,   0,   0,
    108,  16,   0,   0,   6,   0,
      0,   0,  56,   0,   0,   0,
    236,   4,   0,   0, 152,  12,
      0,   0,  20,  13,   0,   0,
    200,  15,   0,   0,  56,  16,
      0,   0,  65, 111, 110,  57,
    172,   4,   0,   0, 172,   4,
      0,   0,   0,   2, 255, 255,
     84,   4,   0,   0,  88,   0,
      0,   0,   4,   0,  40,   0,
      0,   0,  88,   0,   0,   0,
     88,   0,   1,   0,  36,   0,
      0,   0,  88,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      3,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   4,   0,
      5,   0,   3,   0,   0,   0,
      0,   0,   0,   0,  10,   0,
      6,   0,   8,   0,   0,   0,
      0,   0,   0,   0,  16,   0,
      1,   0,  14,   0,   2,   2,
      0,   0,   1,   2, 255, 255,
     81,   0,   0,   5,  15,   0,
     15, 160,   0,   0,   0,  63,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
     81,   0,   0,   5,  16,   0,
     15, 160,   0,   0,   0,  64,
      0,   0, 128, 191,   0,   0,
    128,  63,   0,   0,   0, 128,
     31,   0,   0,   2,   0,   0,
      0, 128,   0,   0,  15, 176,
     31,   0,   0,   2,   0,   0,
      0, 144,   0,   8,  15, 160,
      4,   0,   0,   4,   0,   0,
      8, 128,   0,   0,   0, 176,
     16,   0,   0, 160,  16,   0,
     85, 160,  35,   0,   0,   2,
      0,   0,   1, 128,   0,   0,
    255, 128,   2,   0,   0,   3,
      0,   0,   1, 128,   0,   0,
      0, 129,  16,   0, 170, 160,
      5,   0,   0,   3,   0,   0,
      1, 128,   0,   0,   0, 128,
     13,   0,   0, 160,  10,   0,
      0,   3,   1,   0,   8, 128,
      0,   0,   0, 128,  16,   0,
    170, 160,  10,   0,   0,   3,
      0,   0,   1, 128,   0,   0,
     85, 176,  16,   0, 170, 160,
      4,   0,   0,   4,   0,   0,
      2, 128,   1,   0, 255, 128,
      0,   0,   0, 128,  13,   0,
     85, 161,   5,   0,   0,   3,
      0,   0,   1, 128,   0,   0,
      0, 128,   1,   0, 255, 128,
     88,   0,   0,   4,   1,   0,
     15, 128,   0,   0,  85, 128,
     16,   0, 255, 160,  16,   0,
     85, 160,  65,   0,   0,   1,
      1,   0,  15, 128,   5,   0,
      0,   3,   0,   0,   6, 128,
      0,   0, 170, 176,   1,   0,
    208, 160,   4,   0,   0,   4,
      0,   0,   6, 128,   0,   0,
    208, 160,   0,   0, 255, 176,
      0,   0, 228, 128,   2,   0,
      0,   3,   0,   0,   6, 128,
      0,   0, 228, 128,   2,   0,
    208, 160,  35,   0,   0,   2,
      0,   0,   6, 128,   0,   0,
    228, 128,   2,   0,   0,   3,
      0,   0,   6, 128,   0,   0,
    228, 128,   3,   0, 208, 161,
      1,   0,   0,   2,   0,   0,
      8, 128,  15,   0,   0, 160,
      4,   0,   0,   4,   0,   0,
     22, 128,   0,   0, 228, 128,
      4,   0, 208, 161,   0,   0,
    255, 128,   5,   0,   0,   3,
      0,   0,   2, 128,   0,   0,
    170, 128,   0,   0,  85, 128,
      5,   0,   0,   3,   0,   0,
      4, 128,  14,   0,  85, 160,
     14,   0,  85, 160,   5,   0,
      0,   3,   1,   0,   3, 128,
      0,   0, 170, 176,   6,   0,
    228, 160,   4,   0,   0,   4,
      1,   0,   3, 128,   5,   0,
    228, 160,   0,   0, 255, 176,
      1,   0, 228, 128,   2,   0,
      0,   3,   1,   0,   3, 128,
      1,   0, 228, 128,   7,   0,
    228, 160,   1,   0,   0,   2,
      2,   0,   3, 128,   8,   0,
    228, 160,   2,   0,   0,   3,
      1,   0,  12, 128,   2,   0,
     68, 128,   9,   0,   0, 161,
     35,   0,   0,   2,   2,   0,
      3, 128,   1,   0, 228, 128,
      2,   0,   0,   3,   1,   0,
     12, 128,   1,   0, 228, 129,
      2,   0,  68, 128,  11,   0,
      0,   3,   2,   0,   3, 128,
      1,   0, 238, 128,  16,   0,
    255, 161,  90,   0,   0,   4,
      2,   0,   1, 128,   2,   0,
    228, 128,   2,   0, 228, 128,
     16,   0, 255, 161,   7,   0,
      0,   2,   2,   0,   1, 128,
      2,   0,   0, 128,   6,   0,
      0,   2,   2,   0,   1, 128,
      2,   0,   0, 128,  11,   0,
      0,   3,   2,   0,   2, 128,
      1,   0, 170, 128,   1,   0,
    255, 128,  10,   0,   0,   3,
      1,   0,   4, 128,   2,   0,
     85, 128,  16,   0, 255, 161,
      2,   0,   0,   3,   1,   0,
      4, 128,   2,   0,   0, 128,
      1,   0, 170, 128,   2,   0,
      0,   3,   1,   0,   4, 128,
      1,   0, 170, 128,   9,   0,
      0, 161,   4,   0,   0,   4,
      0,   0,   8, 128,  10,   0,
      0, 160,   0,   0, 255, 128,
      1,   0, 170, 128,   6,   0,
      0,   2,   1,   0,   4, 128,
     10,   0,   0, 160,   5,   0,
      0,   3,   0,   0,  24, 128,
      0,   0, 255, 128,   1,   0,
    170, 128,   5,   0,   0,   3,
      0,   0,   1, 128,   0,   0,
     85, 128,   0,   0,   0, 128,
      6,   0,   0,   2,   2,   0,
      1, 128,   8,   0,   0, 160,
      6,   0,   0,   2,   2,   0,
      2, 128,   8,   0,  85, 160,
      5,   0,   0,   3,   1,   0,
      3, 128,   1,   0, 228, 128,
      2,   0, 228, 128,   1,   0,
      0,   2,   2,   0,  15, 128,
     11,   0, 228, 160,   2,   0,
      0,   3,   2,   0,  15, 128,
      2,   0, 228, 129,  12,   0,
    228, 160,   4,   0,   0,   4,
      2,   0,  15, 128,   0,   0,
    255, 128,   2,   0, 228, 128,
     11,   0, 228, 160,   5,   0,
      0,   3,   2,   0,  15, 128,
      0,   0,   0, 128,   2,   0,
    228, 128,   1,   0,   0,   2,
      3,   0,   5, 128,  16,   0,
    228, 160,   2,   0,   0,   3,
      3,   0,  15, 128,   3,   0,
     10, 129,  14,   0,  65, 160,
      5,   0,   0,   3,   3,   0,
     15, 128,   3,   0, 228, 128,
      3,   0, 228, 128,  66,   0,
      0,   3,   4,   0,  15, 128,
      0,   0, 228, 176,   0,   8,
    228, 160,  66,   0,   0,   3,
      1,   0,  15, 128,   1,   0,
    228, 128,   0,   8, 228, 160,
      5,   0,   0,   3,   5,   0,
      7, 128,   1,   0, 255, 128,
      1,   0, 228, 128,  88,   0,
      0,   4,   1,   0,   7, 128,
      3,   0,  85, 129,   5,   0,
    228, 128,   1,   0, 228, 128,
     88,   0,   0,   4,   1,   0,
     14, 128,   3,   0, 170, 129,
      1,   0,   0, 128,   1,   0,
    228, 128,   5,   0,   0,   3,
      1,   0,  15, 128,   1,   0,
    228, 128,  11,   0, 228, 160,
      5,   0,   0,   3,   1,   0,
     15, 128,   0,   0,   0, 128,
      1,   0, 228, 128,   5,   0,
      0,   3,   5,   0,   7, 128,
      4,   0, 255, 128,   4,   0,
    228, 128,  88,   0,   0,   4,
      4,   0,   7, 128,   3,   0,
     85, 129,   5,   0, 228, 128,
      4,   0, 228, 128,  88,   0,
      0,   4,   4,   0,  14, 128,
      3,   0, 170, 129,   4,   0,
      0, 128,   4,   0, 228, 128,
      5,   0,   0,   3,   4,   0,
     15, 128,   0,   0,  85, 128,
      4,   0, 228, 128,   5,   0,
      0,   3,   4,   0,  15, 128,
      4,   0, 228, 128,  11,   0,
    228, 160,  88,   0,   0,   4,
      4,   0,  15, 128,   3,   0,
    255, 129,  16,   0, 170, 160,
      4,   0, 228, 128,  88,   0,
      0,   4,   1,   0,  15, 128,
      3,   0,   0, 129,   1,   0,
    228, 128,   4,   0, 228, 128,
     88,   0,   0,   4,   0,   0,
     15, 128,   0,   0, 170, 129,
      2,   0, 228, 128,   1,   0,
    228, 128,   1,   0,   0,   2,
      0,   8,  15, 128,   0,   0,
    228, 128, 255, 255,   0,   0,
     83,  72,  68,  82, 164,   7,
      0,   0,  64,   0,   0,   0,
    233,   1,   0,   0,  89,   0,
      0,   4,  70, 142,  32,   0,
      0,   0,   0,   0,  17,   0,
      0,   0,  90,   0,   0,   3,
      0,  96,  16,   0,   0,   0,
      0,   0,  88,  24,   0,   4,
      0, 112,  16,   0,   0,   0,
      0,   0,  85,  85,   0,   0,
     98,  16,   0,   3,  50,  16,
     16,   0,   1,   0,   0,   0,
     98,  16,   0,   3, 194,  16,
     16,   0,   1,   0,   0,   0,
    101,   0,   0,   3, 242,  32,
     16,   0,   0,   0,   0,   0,
    104,   0,   0,   2,   3,   0,
      0,   0,  56,   0,   0,   8,
     50,   0,  16,   0,   0,   0,
      0,   0, 246,  31,  16,   0,
      1,   0,   0,   0,  70, 128,
     32,   0,   0,   0,   0,   0,
      1,   0,   0,   0,  50,   0,
      0,  10,  50,   0,  16,   0,
      0,   0,   0,   0,  70, 128,
     32,   0,   0,   0,   0,   0,
      0,   0,   0,   0, 166,  26,
     16,   0,   1,   0,   0,   0,
     70,   0,  16,   0,   0,   0,
      0,   0,   0,   0,   0,   8,
     50,   0,  16,   0,   0,   0,
      0,   0,  70,   0,  16,   0,
      0,   0,   0,   0,  70, 128,
     32,   0,   0,   0,   0,   0,
      2,   0,   0,   0,   0,   0,
      0,  10,  50,   0,  16,   0,
      0,   0,   0,   0,  70,   0,
     16, 128, 129,   0,   0,   0,
      0,   0,   0,   0,  70, 128,
     32, 128,  65,   0,   0,   0,
      0,   0,   0,   0,   4,   0,
      0,   0,  50,  32,   0,  14,
     50,   0,  16,   0,   0,   0,
      0,   0,  70,   0,  16, 128,
     65,   0,   0,   0,   0,   0,
      0,   0,  70, 128,  32,   0,
      0,   0,   0,   0,   5,   0,
      0,   0,   2,  64,   0,   0,
      0,   0,   0,  63,   0,   0,
      0,  63,   0,   0,   0,   0,
      0,   0,   0,   0,  56,   0,
      0,   7,  18,   0,  16,   0,
      0,   0,   0,   0,  26,   0,
     16,   0,   0,   0,   0,   0,
     10,   0,  16,   0,   0,   0,
      0,   0,  50,   0,   0,   9,
     34,   0,  16,   0,   0,   0,
      0,   0,  10,  16,  16,   0,
      1,   0,   0,   0,   1,  64,
      0,   0,   0,   0,   0,  64,
      1,  64,   0,   0,   0,   0,
    128, 191,   0,   0,   0,   8,
     34,   0,  16,   0,   0,   0,
      0,   0,  26,   0,  16, 128,
    193,   0,   0,   0,   0,   0,
      0,   0,   1,  64,   0,   0,
      0,   0, 128,  63,  56,   0,
      0,   8,  34,   0,  16,   0,
      0,   0,   0,   0,  26,   0,
     16,   0,   0,   0,   0,   0,
     10, 128,  32,   0,   0,   0,
      0,   0,  15,   0,   0,   0,
     51,   0,   0,   7,  34,   0,
     16,   0,   0,   0,   0,   0,
     26,   0,  16,   0,   0,   0,
      0,   0,   1,  64,   0,   0,
      0,   0, 128,  63,  51,   0,
      0,   7,  66,   0,  16,   0,
      0,   0,   0,   0,  26,  16,
     16,   0,   1,   0,   0,   0,
      1,  64,   0,   0,   0,   0,
    128,  63,  56,   0,   0,   7,
     34,   0,  16,   0,   0,   0,
      0,   0,  42,   0,  16,   0,
      0,   0,   0,   0,  26,   0,
     16,   0,   0,   0,   0,   0,
     31,   0,   0,   4,  26, 128,
     32,   0,   0,   0,   0,   0,
     16,   0,   0,   0,  56,   0,
      0,   8, 194,   0,  16,   0,
      0,   0,   0,   0, 246,  31,
     16,   0,   1,   0,   0,   0,
      6, 132,  32,   0,   0,   0,
      0,   0,   7,   0,   0,   0,
     50,   0,   0,  10, 194,   0,
     16,   0,   0,   0,   0,   0,
      6, 132,  32,   0,   0,   0,
      0,   0,   6,   0,   0,   0,
    166,  26,  16,   0,   1,   0,
      0,   0, 166,  14,  16,   0,
      0,   0,   0,   0,   0,   0,
      0,   8, 194,   0,  16,   0,
      0,   0,   0,   0, 166,  14,
     16,   0,   0,   0,   0,   0,
      6, 132,  32,   0,   0,   0,
      0,   0,   8,   0,   0,   0,
      0,   0,   0,  10,  50,   0,
     16,   0,   1,   0,   0,   0,
     70, 128,  32,   0,   0,   0,
      0,   0,  10,   0,   0,   0,
      6, 128,  32, 128,  65,   0,
      0,   0,   0,   0,   0,   0,
     11,   0,   0,   0,   0,   0,
      0,   9, 194,   0,  16,   0,
      0,   0,   0,   0, 166,  14,
     16, 128, 129,   0,   0,   0,
      0,   0,   0,   0,   6,   4,
     16, 128,  65,   0,   0,   0,
      1,   0,   0,   0,  52,   0,
      0,   7,  18,   0,  16,   0,
      1,   0,   0,   0,  58,   0,
     16,   0,   0,   0,   0,   0,
     42,   0,  16,   0,   0,   0,
      0,   0,  51,   0,   0,   7,
     18,   0,  16,   0,   1,   0,
      0,   0,  10,   0,  16,   0,
      1,   0,   0,   0,   1,  64,
      0,   0,   0,   0,   0,   0,
     52,   0,   0,  10, 194,   0,
     16,   0,   0,   0,   0,   0,
    166,  14,  16,   0,   0,   0,
      0,   0,   2,  64,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,  15,   0,
      0,   7,  66,   0,  16,   0,
      0,   0,   0,   0, 230,  10,
     16,   0,   0,   0,   0,   0,
    230,  10,  16,   0,   0,   0,
      0,   0,  75,   0,   0,   5,
     66,   0,  16,   0,   0,   0,
      0,   0,  42,   0,  16,   0,
      0,   0,   0,   0,   0,   0,
      0,   7,  66,   0,  16,   0,
      0,   0,   0,   0,  42,   0,
     16,   0,   0,   0,   0,   0,
     10,   0,  16,   0,   1,   0,
      0,   0,   0,   0,   0,   9,
     66,   0,  16,   0,   0,   0,
      0,   0,  42,   0,  16,   0,
      0,   0,   0,   0,  10, 128,
     32, 128,  65,   0,   0,   0,
      0,   0,   0,   0,  11,   0,
      0,   0,  50,   0,   0,  10,
     66,   0,  16,   0,   0,   0,
      0,   0,  10, 128,  32,   0,
      0,   0,   0,   0,  12,   0,
      0,   0,   1,  64,   0,   0,
      0,   0,   0,  63,  42,   0,
     16,   0,   0,   0,   0,   0,
     14,  32,   0,   8,  66,   0,
     16,   0,   0,   0,   0,   0,
     42,   0,  16,   0,   0,   0,
      0,   0,  10, 128,  32,   0,
      0,   0,   0,   0,  12,   0,
      0,   0,   0,   0,   0,  10,
    242,   0,  16,   0,   1,   0,
      0,   0,  70, 142,  32, 128,
     65,   0,   0,   0,   0,   0,
      0,   0,  13,   0,   0,   0,
     70, 142,  32,   0,   0,   0,
      0,   0,  14,   0,   0,   0,
     50,   0,   0,  10, 242,   0,
     16,   0,   1,   0,   0,   0,
    166,  10,  16,   0,   0,   0,
      0,   0,  70,  14,  16,   0,
      1,   0,   0,   0,  70, 142,
     32,   0,   0,   0,   0,   0,
     13,   0,   0,   0,  56,   0,
      0,   7,  66,   0,  16,   0,
      0,   0,   0,   0,  10,   0,
     16,   0,   0,   0,   0,   0,
     26,   0,  16,   0,   0,   0,
      0,   0,  56,   0,   0,   7,
    242,  32,  16,   0,   0,   0,
      0,   0, 166,  10,  16,   0,
      0,   0,   0,   0,  70,  14,
     16,   0,   1,   0,   0,   0,
     18,   0,   0,   1,  32,   0,
      0,   8,  66,   0,  16,   0,
      0,   0,   0,   0,  26, 128,
     32,   0,   0,   0,   0,   0,
     16,   0,   0,   0,   1,  64,
      0,   0,   1,   0,   0,   0,
     31,   0,   4,   3,  42,   0,
     16,   0,   0,   0,   0,   0,
     56,   0,   0,   8, 194,   0,
     16,   0,   0,   0,   0,   0,
    246,  31,  16,   0,   1,   0,
      0,   0,   6, 132,  32,   0,
      0,   0,   0,   0,   7,   0,
      0,   0,  50,   0,   0,  10,
    194,   0,  16,   0,   0,   0,
      0,   0,   6, 132,  32,   0,
      0,   0,   0,   0,   6,   0,
      0,   0, 166,  26,  16,   0,
      1,   0,   0,   0, 166,  14,
     16,   0,   0,   0,   0,   0,
      0,   0,   0,   8, 194,   0,
     16,   0,   0,   0,   0,   0,
    166,  14,  16,   0,   0,   0,
      0,   0,   6, 132,  32,   0,
      0,   0,   0,   0,   8,   0,
      0,   0,  14,   0,   0,   8,
    194,   0,  16,   0,   0,   0,
      0,   0, 166,  14,  16,   0,
      0,   0,   0,   0,   6, 132,
     32,   0,   0,   0,   0,   0,
     10,   0,   0,   0,  69,   0,
      0,   9, 242,   0,  16,   0,
      1,   0,   0,   0, 230,  10,
     16,   0,   0,   0,   0,   0,
     70, 126,  16,   0,   0,   0,
      0,   0,   0,  96,  16,   0,
      0,   0,   0,   0,  32,   0,
      0,  11, 194,   0,  16,   0,
      0,   0,   0,   0,   6, 128,
     32,   0,   0,   0,   0,   0,
     16,   0,   0,   0,   2,  64,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   1,   0,
      0,   0,   2,   0,   0,   0,
     56,   0,   0,   7, 114,   0,
     16,   0,   2,   0,   0,   0,
    246,  15,  16,   0,   1,   0,
      0,   0,  70,   2,  16,   0,
      1,   0,   0,   0,  55,   0,
      0,   9, 114,   0,  16,   0,
      1,   0,   0,   0, 166,  10,
     16,   0,   0,   0,   0,   0,
     70,   2,  16,   0,   2,   0,
      0,   0,  70,   2,  16,   0,
      1,   0,   0,   0,  55,   0,
      0,   9, 226,   0,  16,   0,
      1,   0,   0,   0, 246,  15,
     16,   0,   0,   0,   0,   0,
      6,   0,  16,   0,   1,   0,
      0,   0,  86,  14,  16,   0,
      1,   0,   0,   0,  56,   0,
      0,   8, 242,   0,  16,   0,
      1,   0,   0,   0,  70,  14,
     16,   0,   1,   0,   0,   0,
     70, 142,  32,   0,   0,   0,
      0,   0,  13,   0,   0,   0,
     56,   0,   0,   7,  66,   0,
     16,   0,   0,   0,   0,   0,
     10,   0,  16,   0,   0,   0,
      0,   0,  26,   0,  16,   0,
      0,   0,   0,   0,  56,   0,
      0,   7, 242,  32,  16,   0,
      0,   0,   0,   0, 166,  10,
     16,   0,   0,   0,   0,   0,
     70,  14,  16,   0,   1,   0,
      0,   0,  18,   0,   0,   1,
     32,   0,   0,   8,  66,   0,
     16,   0,   0,   0,   0,   0,
     26, 128,  32,   0,   0,   0,
      0,   0,  16,   0,   0,   0,
      1,  64,   0,   0,   2,   0,
      0,   0,  31,   0,   4,   3,
     42,   0,  16,   0,   0,   0,
      0,   0,  54,   0,   0,   8,
    242,  32,  16,   0,   0,   0,
      0,   0,   2,  64,   0,   0,
      0,   0, 128,  63,   0,   0,
    128,  63,   0,   0, 128,  63,
      0,   0, 128,  63,  18,   0,
      0,   1,  69,   0,   0,   9,
    242,   0,  16,   0,   1,   0,
      0,   0,  70,  16,  16,   0,
      1,   0,   0,   0,  70, 126,
     16,   0,   0,   0,   0,   0,
      0,  96,  16,   0,   0,   0,
      0,   0,  32,   0,   0,  11,
    194,   0,  16,   0,   0,   0,
      0,   0,   6, 128,  32,   0,
      0,   0,   0,   0,  16,   0,
      0,   0,   2,  64,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   0,   0,
      2,   0,   0,   0,  56,   0,
      0,   7, 114,   0,  16,   0,
      2,   0,   0,   0, 246,  15,
     16,   0,   1,   0,   0,   0,
     70,   2,  16,   0,   1,   0,
      0,   0,  55,   0,   0,   9,
    114,   0,  16,   0,   1,   0,
      0,   0, 166,  10,  16,   0,
      0,   0,   0,   0,  70,   2,
     16,   0,   2,   0,   0,   0,
     70,   2,  16,   0,   1,   0,
      0,   0,  55,   0,   0,   9,
    226,   0,  16,   0,   1,   0,
      0,   0, 246,  15,  16,   0,
      0,   0,   0,   0,   6,   0,
     16,   0,   1,   0,   0,   0,
     86,  14,  16,   0,   1,   0,
      0,   0,  56,   0,   0,   7,
    242,   0,  16,   0,   1,   0,
      0,   0,   6,   0,  16,   0,
      0,   0,   0,   0,  70,  14,
     16,   0,   1,   0,   0,   0,
     56,   0,   0,   8, 242,  32,
     16,   0,   0,   0,   0,   0,
     70,  14,  16,   0,   1,   0,
      0,   0,  70, 142,  32,   0,
      0,   0,   0,   0,  13,   0,
      0,   0,  21,   0,   0,   1,
     21,   0,   0,   1,  21,   0,
      0,   1,  49,   0,   0,   8,
     18,   0,  16,   0,   0,   0,
      0,   0,  26,   0,  16,   0,
      0,   0,   0,   0,  26, 128,
     32,   0,   0,   0,   0,   0,
     15,   0,   0,   0,  13,   0,
      4,   3,  10,   0,  16,   0,
      0,   0,   0,   0,  62,   0,
      0,   1,  83,  84,  65,  84,
    116,   0,   0,   0,  64,   0,
      0,   0,   3,   0,   0,   0,
      0,   0,   0,   0,   3,   0,
      0,   0,  42,   0,   0,   0,
      4,   0,   0,   0,   0,   0,
      0,   0,   5,   0,   0,   0,
      2,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      2,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   0,   0,
      4,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
     82,  68,  69,  70, 172,   2,
      0,   0,   1,   0,   0,   0,
    160,   0,   0,   0,   3,   0,
      0,   0,  28,   0,   0,   0,
      0,   4, 255, 255,   0,   1,
      0,   0, 121,   2,   0,   0,
    124,   0,   0,   0,   3,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      1,   0,   0,   0,   1,   0,
      0,   0, 134,   0,   0,   0,
      2,   0,   0,   0,   5,   0,
      0,   0,   4,   0,   0,   0,
    255, 255, 255, 255,   0,   0,
      0,   0,   1,   0,   0,   0,
     13,   0,   0,   0, 144,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   1,   0,
      0,   0,   0,   0,   0,   0,
    103,  95, 115,  97, 109, 112,
    108, 101, 114,   0, 103,  95,
    116, 101, 120, 116, 117, 114,
    101,   0,  80,  83,  95,  67,
     79,  78,  83,  84,  65,  78,
     84,  83,   0, 171, 171, 171,
    144,   0,   0,   0,  12,   0,
      0,   0, 184,   0,   0,   0,
     16,   1,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
    216,   1,   0,   0,   0,   0,
      0,   0,  64,   0,   0,   0,
      2,   0,   0,   0, 228,   1,
      0,   0,   0,   0,   0,   0,
    244,   1,   0,   0,  64,   0,
      0,   0,  16,   0,   0,   0,
      2,   0,   0,   0,   0,   2,
      0,   0,   0,   0,   0,   0,
     16,   2,   0,   0,  80,   0,
      0,   0,  16,   0,   0,   0,
      2,   0,   0,   0,   0,   2,
      0,   0,   0,   0,   0,   0,
     29,   2,   0,   0,  96,   0,
      0,   0,  64,   0,   0,   0,
      2,   0,   0,   0, 228,   1,
      0,   0,   0,   0,   0,   0,
     38,   2,   0,   0, 160,   0,
      0,   0,  16,   0,   0,   0,
      2,   0,   0,   0,   0,   2,
      0,   0,   0,   0,   0,   0,
     45,   2,   0,   0, 176,   0,
      0,   0,  16,   0,   0,   0,
      2,   0,   0,   0,   0,   2,
      0,   0,   0,   0,   0,   0,
     52,   2,   0,   0, 192,   0,
      0,   0,  16,   0,   0,   0,
      2,   0,   0,   0,   0,   2,
      0,   0,   0,   0,   0,   0,
     60,   2,   0,   0, 208,   0,
      0,   0,  16,   0,   0,   0,
      2,   0,   0,   0,   0,   2,
      0,   0,   0,   0,   0,   0,
     69,   2,   0,   0, 224,   0,
      0,   0,  16,   0,   0,   0,
      2,   0,   0,   0,   0,   2,
      0,   0,   0,   0,   0,   0,
     78,   2,   0,   0, 240,   0,
      0,   0,  16,   0,   0,   0,
      2,   0,   0,   0,   0,   2,
      0,   0,   0,   0,   0,   0,
     89,   2,   0,   0,   0,   1,
      0,   0,   4,   0,   0,   0,
      2,   0,   0,   0, 100,   2,
      0,   0,   0,   0,   0,   0,
    116,   2,   0,   0,   4,   1,
      0,   0,   4,   0,   0,   0,
      2,   0,   0,   0, 100,   2,
      0,   0,   0,   0,   0,   0,
    115,  99, 105, 115, 115, 111,
    114,  77,  97, 116,   0, 171,
      3,   0,   3,   0,   4,   0,
      4,   0,   0,   0,   0,   0,
      0,   0,   0,   0, 115,  99,
    105, 115, 115, 111, 114,  69,
    120, 116,   0, 171,   1,   0,
      3,   0,   1,   0,   4,   0,
      0,   0,   0,   0,   0,   0,
      0,   0, 115,  99, 105, 115,
    115, 111, 114,  83,  99,  97,
    108, 101,   0, 112,  97, 105,
    110, 116,  77,  97, 116,   0,
    101, 120, 116, 101, 110, 116,
      0, 114,  97, 100, 105, 117,
    115,   0, 102, 101,  97, 116,
    104, 101, 114,   0, 105, 110,
    110, 101, 114,  67, 111, 108,
      0, 111, 117, 116, 101, 114,
     67, 111, 108,   0, 115, 116,
    114, 111, 107, 101,  77, 117,
    108, 116,   0, 116, 101, 120,
     84, 121, 112, 101,   0, 171,
    171, 171,   0,   0,   2,   0,
      1,   0,   1,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
    116, 121, 112, 101,   0,  77,
    105,  99, 114, 111, 115, 111,
    102, 116,  32,  40,  82,  41,
     32,  72,  76,  83,  76,  32,
     83, 104,  97, 100, 101, 114,
     32,  67, 111, 109, 112, 105,
    108, 101, 114,  32,  54,  46,
     51,  46,  57,  54,  48,  48,
     46,  49,  54,  51,  56,  52,
      0, 171,  73,  83,  71,  78,
    104,   0,   0,   0,   3,   0,
      0,   0,   8,   0,   0,   0,
     80,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   0,   0,
      3,   0,   0,   0,   0,   0,
      0,   0,  15,   0,   0,   0,
     92,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      3,   0,   0,   0,   1,   0,
      0,   0,   3,   3,   0,   0,
     92,   0,   0,   0,   1,   0,
      0,   0,   0,   0,   0,   0,
      3,   0,   0,   0,   1,   0,
      0,   0,  12,  12,   0,   0,
     83,  86,  95,  80, 111, 115,
    105, 116, 105, 111, 110,   0,
     84,  69,  88,  67,  79,  79,
     82,  68,   0, 171, 171, 171,
     79,  83,  71,  78,  44,   0,
      0,   0,   1,   0,   0,   0,
      8,   0,   0,   0,  32,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   3,   0,
      0,   0,   0,   0,   0,   0,
     15,   0,   0,   0,  83,  86,
     95,  84,  65,  82,  71,  69,
     84,   0, 171, 171
};
