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
 16;
 0.07384;1.33533;1.38284;,
 0.00000;-0.10917;2.37772;,
 0.69748;-0.10917;1.76890;,
 2.94219;-0.10917;-1.48522;,
 3.07971;-0.10917;-2.26450;,
 2.51450;1.33533;-1.57866;,
 0.32908;1.33533;-2.05616;,
 0.43025;-0.10917;-3.41939;,
 -1.05318;-0.10917;-2.35816;,
 -1.30703;-0.10917;1.59350;,
 -1.30703;1.33533;0.52801;,
 -2.33135;-0.10917;-0.10609;,
 -2.15127;-0.10917;1.08695;,
 -1.30703;-0.10917;-2.17656;,
 -1.30703;1.33533;-0.85690;,
 0.43050;-0.10917;-3.41950;;
 
 16;
 3;0,1,2;,
 3;3,4,5;,
 4;3,5,0,2;,
 3;6,0,5;,
 4;6,5,4,7;,
 3;6,7,8;,
 4;9,1,0,10;,
 4;9,10,11,12;,
 4;13,14,6,8;,
 3;14,11,10;,
 4;14,10,0,6;,
 4;3,2,1,7;,
 4;4,3,7,15;,
 4;1,9,12,11;,
 4;1,11,8,7;,
 3;13,11,14;;
 
 MeshMaterialList {
  1;
  16;
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
   0.360800;0.351200;0.310400;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
 MeshNormals {
  18;
  0.591625;0.436570;0.677781;,
  0.942517;0.289825;0.166322;,
  -0.117365;0.638941;-0.760250;,
  -0.680242;0.573969;0.455884;,
  0.671931;0.401298;0.622470;,
  -0.835778;0.548831;0.016128;,
  0.865630;0.334210;0.372811;,
  -0.158089;0.866551;-0.473390;,
  -0.471218;0.591044;-0.654691;,
  -0.570522;0.560355;0.600422;,
  -0.671226;0.499679;-0.547518;,
  -0.551754;0.763222;0.336245;,
  -0.608739;0.732265;-0.305329;,
  0.000000;-1.000000;0.000000;,
  -0.165697;0.947961;0.271873;,
  0.140850;0.894946;-0.423358;,
  0.252107;0.601858;-0.757766;,
  -0.441909;0.528184;0.725078;;
  16;
  3;4,0,4;,
  3;6,1,6;,
  4;6,6,4,4;,
  3;7,14,15;,
  4;7,15,16,2;,
  3;7,2,8;,
  4;9,17,14,11;,
  4;9,11,5,3;,
  4;10,12,7,8;,
  3;12,5,11;,
  4;12,11,14,7;,
  4;13,13,13,13;,
  4;13,13,13,13;,
  4;13,13,13,13;,
  4;13,13,13,13;,
  3;10,5,12;;
 }
 MeshTextureCoords {
  16;
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
