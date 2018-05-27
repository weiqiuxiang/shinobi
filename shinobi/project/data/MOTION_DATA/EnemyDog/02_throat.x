xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 66;
 -0.09640;1.80789;1.21588;,
 0.26288;1.69357;1.31695;,
 0.24645;0.27244;-0.23215;,
 -0.11283;0.38676;-0.33322;,
 0.19012;1.64127;1.36569;,
 -0.09731;1.73273;1.28484;,
 -0.11373;0.31161;-0.26426;,
 0.17369;0.22015;-0.18341;,
 0.40906;1.42609;1.56078;,
 0.39263;0.00496;0.01168;,
 0.30706;1.42729;1.56076;,
 0.29064;0.00616;0.01166;,
 0.25650;1.16213;1.80455;,
 0.24007;-0.25900;0.25545;,
 0.18502;1.21612;1.75577;,
 0.16859;-0.20500;0.20668;,
 -0.10543;1.05632;1.90546;,
 -0.12186;-0.36481;0.35636;,
 -0.10453;1.13148;1.83650;,
 -0.12096;-0.28965;0.28740;,
 -0.46472;1.17064;1.80439;,
 -0.48115;-0.25048;0.25529;,
 -0.39196;1.22293;1.75564;,
 -0.40839;-0.19819;0.20655;,
 -0.61089;1.43812;1.56055;,
 -0.62732;0.01700;0.01146;,
 -0.50890;1.43692;1.56058;,
 -0.52533;0.01579;0.01148;,
 -0.45833;1.70208;1.31678;,
 -0.47476;0.28096;-0.23231;,
 -0.38685;1.64809;1.36556;,
 -0.40328;0.22696;-0.18354;,
 -0.09640;1.80789;1.21588;,
 -0.11283;0.38676;-0.33322;,
 -0.09731;1.73273;1.28484;,
 -0.11373;0.31161;-0.26426;,
 0.19012;1.64127;1.36569;,
 0.26288;1.69357;1.31695;,
 -0.11283;0.38676;-0.33322;,
 0.24645;0.27244;-0.23215;,
 0.17369;0.22015;-0.18341;,
 -0.11373;0.31161;-0.26426;,
 0.30706;1.42729;1.56076;,
 0.40906;1.42609;1.56078;,
 0.39263;0.00496;0.01168;,
 0.29064;0.00616;0.01166;,
 0.18502;1.21612;1.75577;,
 0.25650;1.16213;1.80455;,
 0.24007;-0.25900;0.25545;,
 0.16859;-0.20500;0.20668;,
 -0.10453;1.13148;1.83650;,
 -0.10543;1.05632;1.90546;,
 -0.12186;-0.36481;0.35636;,
 -0.12096;-0.28965;0.28740;,
 -0.39196;1.22293;1.75564;,
 -0.46472;1.17064;1.80439;,
 -0.48115;-0.25048;0.25529;,
 -0.40839;-0.19819;0.20655;,
 -0.50890;1.43692;1.56058;,
 -0.61089;1.43812;1.56055;,
 -0.62732;0.01700;0.01146;,
 -0.52533;0.01579;0.01148;,
 -0.38685;1.64809;1.36556;,
 -0.45833;1.70208;1.31678;,
 -0.47476;0.28096;-0.23231;,
 -0.40328;0.22696;-0.18354;;
 
 32;
 4;0,1,2,3;,
 4;4,5,6,7;,
 4;1,8,9,2;,
 4;10,4,7,11;,
 4;8,12,13,9;,
 4;14,10,11,15;,
 4;12,16,17,13;,
 4;18,14,15,19;,
 4;16,20,21,17;,
 4;22,18,19,23;,
 4;20,24,25,21;,
 4;26,22,23,27;,
 4;24,28,29,25;,
 4;30,26,27,31;,
 4;28,32,33,29;,
 4;34,30,31,35;,
 4;0,5,36,37;,
 4;38,39,40,41;,
 4;37,36,42,43;,
 4;39,44,45,40;,
 4;43,42,46,47;,
 4;44,48,49,45;,
 4;47,46,50,51;,
 4;48,52,53,49;,
 4;51,50,54,55;,
 4;52,56,57,53;,
 4;55,54,58,59;,
 4;56,60,61,57;,
 4;59,58,62,63;,
 4;60,64,65,61;,
 4;63,62,5,0;,
 4;64,38,41,65;;
 
 MeshMaterialList {
  1;
  32;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   0.800000;0.790400;0.382400;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
    TextureFilename {
    "data/TEXTURE/ShaderDummy.png";
   }
  }
 }
 MeshNormals {
  32;
  0.008849;0.736813;-0.676038;,
  0.713315;0.512659;-0.477872;,
  0.999930;-0.011804;0.000224;,
  0.700800;-0.529352;0.478190;,
  -0.008849;-0.736813;0.676038;,
  -0.713315;-0.512659;0.477873;,
  -0.999930;0.011804;-0.000225;,
  -0.700800;0.529352;-0.478190;,
  -0.008850;-0.736814;0.676038;,
  -0.713315;-0.512660;0.477872;,
  -0.999930;0.011803;-0.000224;,
  -0.700800;0.529352;-0.478190;,
  0.008850;0.736813;-0.676038;,
  0.713315;0.512659;-0.477872;,
  0.999930;-0.011804;0.000225;,
  0.700800;-0.529352;0.478190;,
  0.007815;0.675994;0.736866;,
  0.007816;0.675995;0.736865;,
  -0.007815;-0.675994;-0.736866;,
  -0.007815;-0.675994;-0.736865;,
  0.007815;0.675994;0.736866;,
  -0.007815;-0.675993;-0.736866;,
  0.007812;0.675995;0.736865;,
  -0.007815;-0.675993;-0.736867;,
  0.007814;0.675995;0.736864;,
  -0.007815;-0.675994;-0.736866;,
  0.007815;0.675994;0.736866;,
  -0.007813;-0.675993;-0.736866;,
  0.007814;0.675993;0.736866;,
  -0.007812;-0.675993;-0.736866;,
  0.007814;0.675994;0.736866;,
  -0.007814;-0.675994;-0.736866;;
  32;
  4;0,1,1,0;,
  4;9,8,8,9;,
  4;1,2,2,1;,
  4;10,9,9,10;,
  4;2,3,3,2;,
  4;11,10,10,11;,
  4;3,4,4,3;,
  4;12,11,11,12;,
  4;4,5,5,4;,
  4;13,12,12,13;,
  4;5,6,6,5;,
  4;14,13,13,14;,
  4;6,7,7,6;,
  4;15,14,14,15;,
  4;7,0,0,7;,
  4;8,15,15,8;,
  4;16,16,17,17;,
  4;18,19,19,18;,
  4;17,17,20,20;,
  4;19,21,21,19;,
  4;20,20,22,22;,
  4;21,23,23,21;,
  4;22,22,24,24;,
  4;23,25,25,23;,
  4;24,24,26,26;,
  4;25,27,27,25;,
  4;26,26,28,28;,
  4;27,29,29,27;,
  4;28,28,30,30;,
  4;29,31,31,29;,
  4;30,30,16,16;,
  4;31,18,18,31;;
 }
 MeshTextureCoords {
  66;
  0.000000;0.000000;,
  0.125000;0.000000;,
  0.125000;1.000000;,
  0.000000;1.000000;,
  0.125000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  0.125000;1.000000;,
  0.250000;0.000000;,
  0.250000;1.000000;,
  0.250000;0.000000;,
  0.250000;1.000000;,
  0.375000;0.000000;,
  0.375000;1.000000;,
  0.375000;0.000000;,
  0.375000;1.000000;,
  0.500000;0.000000;,
  0.500000;1.000000;,
  0.500000;0.000000;,
  0.500000;1.000000;,
  0.625000;0.000000;,
  0.625000;1.000000;,
  0.625000;0.000000;,
  0.625000;1.000000;,
  0.750000;0.000000;,
  0.750000;1.000000;,
  0.750000;0.000000;,
  0.750000;1.000000;,
  0.875000;0.000000;,
  0.875000;1.000000;,
  0.875000;0.000000;,
  0.875000;1.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;;
 }
}