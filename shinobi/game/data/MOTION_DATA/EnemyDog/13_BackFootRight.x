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
 70;
 -0.03081;0.14312;-0.13698;,
 -0.11094;0.12057;-0.05685;,
 -0.03081;0.12057;-0.02366;,
 -0.03081;0.14312;-0.13698;,
 -0.14413;0.12057;-0.13698;,
 -0.03081;0.14312;-0.13698;,
 -0.11094;0.12057;-0.21712;,
 -0.03081;0.14312;-0.13698;,
 -0.03081;0.12057;-0.25030;,
 -0.17888;0.05638;0.01108;,
 -0.03081;0.05638;0.07241;,
 -0.24021;0.05638;-0.13698;,
 -0.17888;0.05638;-0.28505;,
 -0.03081;0.05638;-0.34638;,
 -0.30440;-0.03969;-0.13698;,
 -0.23187;-0.06229;0.14578;,
 -0.22427;-0.03969;-0.33044;,
 -0.03081;-0.03969;-0.41057;,
 -0.32694;-0.15301;-0.13698;,
 -0.24021;-0.15301;0.15408;,
 -0.24021;-0.15301;-0.34638;,
 -0.03081;-0.15301;-0.43311;,
 -0.15275;-0.04346;0.18550;,
 -0.05727;-0.05359;0.27900;,
 -0.03081;0.00135;0.17763;,
 -0.18024;-0.15301;0.22252;,
 -0.24370;-0.15130;0.32924;,
 -0.08390;-0.15301;0.29948;,
 -0.14885;-0.15179;0.35527;,
 -0.24295;-0.08317;0.32859;,
 -0.23187;-0.06229;0.14578;,
 -0.15275;-0.04346;0.18550;,
 -0.14848;-0.08366;0.35455;,
 -0.05727;-0.05359;0.27900;,
 0.02892;0.12057;-0.02366;,
 0.10906;0.12057;-0.05685;,
 0.02892;0.14312;-0.13698;,
 0.14225;0.12057;-0.13698;,
 0.02892;0.14312;-0.13698;,
 0.10906;0.12057;-0.21712;,
 0.02892;0.14312;-0.13698;,
 0.02892;0.12057;-0.25030;,
 0.02892;0.14312;-0.13698;,
 0.02892;0.05638;0.07241;,
 0.17699;0.05638;0.01108;,
 0.23832;0.05638;-0.13698;,
 0.17699;0.05638;-0.28505;,
 0.02892;0.05638;-0.34638;,
 0.22998;-0.06229;0.14578;,
 0.30251;-0.03969;-0.13698;,
 0.22238;-0.03969;-0.33044;,
 0.02892;-0.03969;-0.41057;,
 0.23832;-0.15301;0.15408;,
 0.32505;-0.15301;-0.13698;,
 0.23832;-0.15301;-0.34638;,
 0.02892;-0.15301;-0.43311;,
 0.02892;0.00135;0.17763;,
 0.05538;-0.05359;0.27900;,
 0.15087;-0.04346;0.18550;,
 0.24181;-0.15130;0.32924;,
 0.17836;-0.15301;0.22252;,
 0.14696;-0.15179;0.35527;,
 0.08202;-0.15301;0.29948;,
 0.22998;-0.06229;0.14578;,
 0.24107;-0.08317;0.32859;,
 0.15087;-0.04346;0.18550;,
 0.14659;-0.08366;0.35455;,
 0.05538;-0.05359;0.27900;,
 -0.00105;-0.15301;0.43451;,
 -0.00105;-0.09508;0.43451;;
 
 82;
 3;0,1,2;,
 3;3,4,1;,
 3;5,6,4;,
 3;7,8,6;,
 4;2,1,9,10;,
 4;1,4,11,9;,
 4;4,6,12,11;,
 4;6,8,13,12;,
 4;9,11,14,15;,
 4;11,12,16,14;,
 4;12,13,17,16;,
 4;15,14,18,19;,
 4;14,16,20,18;,
 4;16,17,21,20;,
 3;19,18,20;,
 3;22,23,24;,
 4;22,24,10,9;,
 3;22,9,15;,
 3;25,26,19;,
 3;27,28,25;,
 4;29,30,19,26;,
 4;31,29,26,25;,
 4;30,31,25,19;,
 3;30,29,31;,
 4;32,31,25,28;,
 4;33,32,28,27;,
 4;31,33,27,25;,
 3;31,32,33;,
 3;34,35,36;,
 3;35,37,38;,
 3;37,39,40;,
 3;39,41,42;,
 4;43,44,35,34;,
 4;44,45,37,35;,
 4;45,46,39,37;,
 4;46,47,41,39;,
 4;48,49,45,44;,
 4;49,50,46,45;,
 4;50,51,47,46;,
 4;52,53,49,48;,
 4;53,54,50,49;,
 4;54,55,51,50;,
 3;54,53,52;,
 3;56,57,58;,
 4;44,43,56,58;,
 3;48,44,58;,
 3;52,59,60;,
 3;60,61,62;,
 4;59,52,63,64;,
 4;60,59,64,65;,
 4;52,60,65,63;,
 3;65,64,63;,
 4;61,60,65,66;,
 4;62,61,66,67;,
 4;60,62,67,65;,
 3;67,66,65;,
 4;2,34,36,0;,
 4;7,42,41,8;,
 4;10,43,34,2;,
 4;8,41,47,13;,
 4;13,47,51,17;,
 4;17,51,55,21;,
 4;21,55,54,20;,
 4;20,54,52,19;,
 4;23,57,56,24;,
 4;24,56,43,10;,
 4;30,63,65,31;,
 4;19,52,63,30;,
 4;31,65,67,33;,
 3;52,60,19;,
 3;19,60,52;,
 3;19,25,60;,
 3;60,25,19;,
 3;60,62,25;,
 3;25,62,60;,
 3;25,27,62;,
 3;62,27,25;,
 3;62,68,27;,
 4;69,33,27,68;,
 4;67,69,68,62;,
 4;33,67,62,27;,
 3;33,69,67;;
 
 MeshMaterialList {
  1;
  82;
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
  83;
  -0.093255;0.995642;-0.000002;,
  -0.078014;0.921488;-0.380492;,
  -0.078020;0.921486;0.380495;,
  -0.269136;0.924733;0.269136;,
  -0.380614;0.924734;0.000001;,
  -0.269129;0.924736;-0.269135;,
  -0.141585;0.701007;-0.698958;,
  -0.118926;0.849706;0.513670;,
  -0.458118;0.784007;0.418880;,
  -0.697107;0.716885;-0.010879;,
  -0.498498;0.709223;-0.498501;,
  -0.181780;0.377192;-0.908120;,
  -0.106349;0.932540;0.345048;,
  -0.783168;0.567665;0.253781;,
  -0.922079;0.384978;-0.039521;,
  -0.652734;0.384548;-0.652733;,
  -0.191748;0.191480;-0.962584;,
  -0.003022;-0.999988;0.003783;,
  -0.987644;0.101330;0.119545;,
  -0.982917;0.175441;-0.055635;,
  -0.693520;0.195103;-0.693517;,
  -0.136839;0.878539;0.457652;,
  -0.279634;0.895745;0.345609;,
  -0.003995;-0.999983;0.004181;,
  -0.010907;-0.999895;0.009555;,
  -0.009066;-0.999894;0.011349;,
  -0.998184;0.048833;-0.035256;,
  -0.976860;0.140643;0.161134;,
  0.093256;0.995642;-0.000002;,
  0.078014;0.921488;-0.380492;,
  0.078020;0.921486;0.380495;,
  0.269137;0.924733;0.269134;,
  0.380616;0.924733;0.000001;,
  0.269130;0.924736;-0.269133;,
  0.141585;0.701007;-0.698958;,
  0.118926;0.849706;0.513670;,
  0.458118;0.784008;0.418879;,
  0.697108;0.716884;-0.010879;,
  0.498499;0.709222;-0.498500;,
  0.181780;0.377192;-0.908120;,
  0.106350;0.932540;0.345048;,
  0.783167;0.567666;0.253781;,
  0.922079;0.384978;-0.039522;,
  0.652735;0.384548;-0.652733;,
  0.191749;0.191480;-0.962584;,
  0.002266;-0.999993;0.002837;,
  0.987644;0.101331;0.119545;,
  0.982916;0.175443;-0.055635;,
  0.693519;0.195106;-0.693516;,
  0.136840;0.878539;0.457652;,
  0.279633;0.895745;0.345610;,
  0.003995;-0.999983;0.004181;,
  0.010907;-0.999895;0.009555;,
  0.009066;-0.999894;0.011349;,
  0.998184;0.048835;-0.035257;,
  0.976860;0.140639;0.161135;,
  0.000000;-1.000000;-0.000000;,
  -0.870419;0.158705;0.466029;,
  -0.002181;-0.999996;0.001911;,
  0.000000;-1.000000;0.000000;,
  0.854803;-0.020013;0.518566;,
  0.627802;-0.262575;-0.732747;,
  0.619228;-0.306317;-0.722998;,
  -0.274366;0.957147;0.092702;,
  0.646099;-0.004500;0.763240;,
  0.324094;-0.293141;-0.899462;,
  -0.125390;0.964470;0.232540;,
  0.002727;-0.999994;0.002389;,
  -0.854804;-0.020024;0.518565;,
  -0.627804;-0.262574;-0.732746;,
  -0.619230;-0.306317;-0.722996;,
  0.274364;0.957147;0.092704;,
  -0.646102;-0.004495;0.763238;,
  -0.324094;-0.293142;-0.899461;,
  0.125391;0.964470;0.232539;,
  0.000000;-0.903599;0.428379;,
  0.000000;-0.986022;0.166616;,
  0.000000;0.091062;0.995845;,
  0.000000;-0.994184;-0.107695;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.869886;0.158744;0.467011;,
  0.000000;0.966190;0.257832;;
  82;
  3;0,3,2;,
  3;0,4,3;,
  3;0,5,4;,
  3;0,1,5;,
  4;2,3,8,7;,
  4;3,4,9,8;,
  4;4,5,10,9;,
  4;5,1,6,10;,
  4;8,9,14,13;,
  4;9,10,15,14;,
  4;10,6,11,15;,
  4;13,14,19,18;,
  4;14,15,20,19;,
  4;15,11,16,20;,
  3;58,59,59;,
  3;22,12,21;,
  4;22,21,7,8;,
  3;22,8,13;,
  3;23,24,58;,
  3;17,25,23;,
  4;26,13,18,26;,
  4;60,60,60,60;,
  4;61,62,62,61;,
  3;13,63,22;,
  4;27,27,27,27;,
  4;64,64,64,64;,
  4;62,65,65,62;,
  3;22,66,12;,
  3;30,31,28;,
  3;31,32,28;,
  3;32,33,28;,
  3;33,29,28;,
  4;35,36,31,30;,
  4;36,37,32,31;,
  4;37,38,33,32;,
  4;38,34,29,33;,
  4;41,42,37,36;,
  4;42,43,38,37;,
  4;43,39,34,38;,
  4;46,47,42,41;,
  4;47,48,43,42;,
  4;48,44,39,43;,
  3;59,59,67;,
  3;49,40,50;,
  4;36,35,49,50;,
  3;41,36,50;,
  3;67,52,51;,
  3;51,53,45;,
  4;54,46,41,54;,
  4;68,68,68,68;,
  4;69,70,70,69;,
  3;50,71,41;,
  4;55,55,55,55;,
  4;72,72,72,72;,
  4;70,73,73,70;,
  3;40,74,50;,
  4;2,30,28,0;,
  4;0,28,29,1;,
  4;7,35,30,2;,
  4;1,29,34,6;,
  4;6,34,39,11;,
  4;11,39,44,16;,
  4;59,59,59,59;,
  4;59,59,67,58;,
  4;12,40,49,21;,
  4;21,49,35,7;,
  4;75,75,76,76;,
  4;77,77,77,77;,
  4;76,76,78,78;,
  3;67,51,58;,
  3;79,79,80;,
  3;79,79,79;,
  3;51,23,58;,
  3;51,45,23;,
  3;79,79,79;,
  3;79,79,79;,
  3;45,17,23;,
  3;45,56,17;,
  4;57,57,57,57;,
  4;81,81,81,81;,
  4;65,73,73,65;,
  3;12,82,40;;
 }
 MeshTextureCoords {
  70;
  0.562500;0.000000;,
  0.625000;0.125000;,
  0.500000;0.125000;,
  0.687500;0.000000;,
  0.750000;0.125000;,
  0.812500;0.000000;,
  0.875000;0.125000;,
  0.937500;0.000000;,
  1.000000;0.125000;,
  0.625000;0.250000;,
  0.500000;0.250000;,
  0.750000;0.250000;,
  0.875000;0.250000;,
  1.000000;0.250000;,
  0.750000;0.375000;,
  0.625000;0.375000;,
  0.875000;0.375000;,
  1.000000;0.375000;,
  0.750000;0.500000;,
  0.625000;0.500000;,
  0.875000;0.500000;,
  1.000000;0.500000;,
  0.561390;0.375000;,
  0.500000;0.375000;,
  0.500000;0.298360;,
  0.589200;0.500000;,
  0.000000;0.000000;,
  0.500000;0.500000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.625000;0.500000;,
  0.589200;0.500000;,
  0.000000;0.000000;,
  0.500000;0.500000;,
  0.500000;0.125000;,
  0.625000;0.125000;,
  0.562500;0.000000;,
  0.750000;0.125000;,
  0.687500;0.000000;,
  0.875000;0.125000;,
  0.812500;0.000000;,
  1.000000;0.125000;,
  0.937500;0.000000;,
  0.500000;0.250000;,
  0.625000;0.250000;,
  0.750000;0.250000;,
  0.875000;0.250000;,
  1.000000;0.250000;,
  0.625000;0.375000;,
  0.750000;0.375000;,
  0.875000;0.375000;,
  1.000000;0.375000;,
  0.625000;0.500000;,
  0.750000;0.500000;,
  0.875000;0.500000;,
  1.000000;0.500000;,
  0.500000;0.298360;,
  0.500000;0.375000;,
  0.561390;0.375000;,
  0.000000;0.000000;,
  0.589200;0.500000;,
  0.000000;0.000000;,
  0.500000;0.500000;,
  0.625000;0.500000;,
  0.000000;0.000000;,
  0.589200;0.500000;,
  0.000000;0.000000;,
  0.500000;0.500000;,
  0.000000;0.000000;,
  0.000000;0.000000;;
 }
}
