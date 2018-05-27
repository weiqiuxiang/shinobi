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
 161;
 0.00000;0.32883;0.00000;,
 0.08898;0.30380;-0.08898;,
 0.00000;0.30380;-0.12584;,
 0.00000;0.32883;0.00000;,
 0.12584;0.30380;0.00000;,
 0.00000;0.32883;0.00000;,
 0.08898;0.30380;0.08898;,
 0.00000;0.32883;0.00000;,
 0.00000;0.30380;0.12584;,
 0.00000;0.32883;0.00000;,
 -0.08898;0.30380;0.08898;,
 0.00000;0.32883;0.00000;,
 -0.12584;0.30380;0.00000;,
 0.00000;0.32883;0.00000;,
 -0.08898;0.30380;-0.08898;,
 0.00000;0.32883;0.00000;,
 0.00000;0.30380;-0.12584;,
 0.16441;0.23251;-0.16441;,
 0.00000;0.23251;-0.23252;,
 0.23252;0.23251;0.00000;,
 0.16441;0.23251;0.16441;,
 0.00000;0.23251;0.23252;,
 -0.16441;0.23251;0.16441;,
 -0.23252;0.23251;0.00000;,
 -0.16441;0.23251;-0.16441;,
 0.00000;0.23251;-0.23252;,
 0.21482;0.12584;-0.21482;,
 0.00000;0.12584;-0.30380;,
 0.30380;0.12584;0.00000;,
 0.21482;0.12584;0.21482;,
 0.00000;0.12584;0.30380;,
 -0.21482;0.12584;0.21482;,
 -0.30380;0.12584;0.00000;,
 -0.21482;0.12584;-0.21482;,
 0.00000;0.12584;-0.30380;,
 0.23252;-0.00000;-0.23252;,
 0.00000;-0.00000;-0.32883;,
 0.32883;-0.00000;0.00000;,
 0.23252;-0.00000;0.23252;,
 0.00000;-0.00000;0.32883;,
 -0.23252;-0.00000;0.23252;,
 -0.32883;-0.00000;0.00000;,
 -0.23252;-0.00000;-0.23252;,
 0.00000;-0.00000;-0.32883;,
 0.21482;-0.12584;-0.21482;,
 0.00000;-0.12584;-0.30380;,
 0.30380;-0.12584;0.00000;,
 0.21482;-0.12584;0.21482;,
 0.00000;-0.12584;0.30380;,
 -0.21482;-0.12584;0.21482;,
 -0.30380;-0.12584;0.00000;,
 -0.21482;-0.12584;-0.21482;,
 0.00000;-0.12584;-0.30380;,
 0.16441;-0.23252;-0.16441;,
 0.00000;-0.23252;-0.23252;,
 0.23252;-0.23252;0.00000;,
 0.16441;-0.23252;0.16441;,
 0.00000;-0.23252;0.23252;,
 -0.16441;-0.23252;0.16441;,
 -0.23252;-0.23252;0.00000;,
 -0.16441;-0.23252;-0.16441;,
 0.00000;-0.23252;-0.23252;,
 0.08898;-0.30380;-0.08898;,
 0.00000;-0.30380;-0.12584;,
 0.12584;-0.30380;0.00000;,
 0.08898;-0.30380;0.08898;,
 0.00000;-0.30380;0.12584;,
 -0.08898;-0.30380;0.08898;,
 -0.12584;-0.30380;0.00000;,
 -0.08898;-0.30380;-0.08898;,
 0.00000;-0.30380;-0.12584;,
 0.00000;-0.32883;-0.00000;,
 0.00000;-0.32883;-0.00000;,
 0.00000;-0.32883;-0.00000;,
 0.00000;-0.32883;-0.00000;,
 0.00000;-0.32883;-0.00000;,
 0.00000;-0.32883;-0.00000;,
 0.00000;-0.32883;-0.00000;,
 0.00000;-0.32883;-0.00000;,
 -1.32276;0.00000;0.00000;,
 -1.32276;0.00000;-0.25916;,
 -1.32276;-0.15936;-0.18325;,
 -1.32276;-0.22538;0.00000;,
 -1.32276;-0.15936;0.18325;,
 -1.32276;0.00000;0.25916;,
 -1.32276;0.15937;0.18325;,
 -1.32276;0.22538;0.00000;,
 -1.32276;0.15937;-0.18325;,
 -1.31379;0.00000;-0.28395;,
 -1.31379;-0.17461;-0.20078;,
 -1.31379;-0.24694;0.00000;,
 -1.31379;-0.17461;0.20078;,
 -1.31379;0.00000;0.28395;,
 -1.31379;0.17461;0.20078;,
 -1.31379;0.24694;0.00000;,
 -1.31379;0.17461;-0.20078;,
 -1.28824;0.00000;-0.30497;,
 -1.28824;-0.18754;-0.21565;,
 -1.28824;-0.26522;0.00000;,
 -1.28824;-0.18754;0.21565;,
 -1.28824;0.00000;0.30497;,
 -1.28824;0.18754;0.21565;,
 -1.28824;0.26522;0.00000;,
 -1.28824;0.18754;-0.21565;,
 -1.25000;0.00000;-0.31901;,
 -1.25000;-0.19617;-0.22558;,
 -1.25000;-0.27743;0.00000;,
 -1.25000;-0.19617;0.22558;,
 -1.25000;0.00000;0.31901;,
 -1.25000;0.19617;0.22558;,
 -1.25000;0.27743;0.00000;,
 -1.25000;0.19617;-0.22558;,
 -1.20489;0.00000;-0.32394;,
 -1.20489;-0.19921;-0.22906;,
 -1.20489;-0.28172;0.00000;,
 -1.20489;-0.19921;0.22906;,
 -1.20489;0.00000;0.32394;,
 -1.20489;0.19921;0.22906;,
 -1.20489;0.28172;0.00000;,
 -1.20489;0.19921;-0.22906;,
 -0.26188;0.00000;-0.32394;,
 -0.26188;-0.19921;-0.22906;,
 -0.26188;-0.28172;0.00000;,
 -0.26188;-0.19921;0.22906;,
 -0.26188;0.00000;0.32394;,
 -0.26188;0.19921;0.22906;,
 -0.26188;0.28172;0.00000;,
 -0.26188;0.19921;-0.22906;,
 -0.21677;0.00000;-0.31901;,
 -0.21677;-0.19617;-0.22558;,
 -0.21677;-0.27743;0.00000;,
 -0.21677;-0.19617;0.22558;,
 -0.21677;0.00000;0.31901;,
 -0.21677;0.19617;0.22558;,
 -0.21677;0.27743;0.00000;,
 -0.21677;0.19617;-0.22558;,
 -0.17852;0.00000;-0.30497;,
 -0.17852;-0.18754;-0.21565;,
 -0.17852;-0.26522;0.00000;,
 -0.17852;-0.18754;0.21565;,
 -0.17852;0.00000;0.30497;,
 -0.17852;0.18754;0.21565;,
 -0.17852;0.26522;0.00000;,
 -0.17852;0.18754;-0.21565;,
 -0.15297;0.00000;-0.28395;,
 -0.15297;-0.17461;-0.20078;,
 -0.15297;-0.24694;0.00000;,
 -0.15297;-0.17461;0.20078;,
 -0.15297;0.00000;0.28395;,
 -0.15297;0.17461;0.20078;,
 -0.15297;0.24694;0.00000;,
 -0.15297;0.17461;-0.20078;,
 -0.14400;0.00000;-0.25916;,
 -0.14400;-0.15936;-0.18325;,
 -0.14400;-0.22538;0.00000;,
 -0.14400;-0.15936;0.18325;,
 -0.14400;0.00000;0.25916;,
 -0.14400;0.15937;0.18325;,
 -0.14400;0.22538;0.00000;,
 -0.14400;0.15937;-0.18325;,
 -0.14400;0.00000;0.00000;;
 
 152;
 3;0,1,2;,
 3;3,4,1;,
 3;5,6,4;,
 3;7,8,6;,
 3;9,10,8;,
 3;11,12,10;,
 3;13,14,12;,
 3;15,16,14;,
 4;2,1,17,18;,
 4;1,4,19,17;,
 4;4,6,20,19;,
 4;6,8,21,20;,
 4;8,10,22,21;,
 4;10,12,23,22;,
 4;12,14,24,23;,
 4;14,16,25,24;,
 4;18,17,26,27;,
 4;17,19,28,26;,
 4;19,20,29,28;,
 4;20,21,30,29;,
 4;21,22,31,30;,
 4;22,23,32,31;,
 4;23,24,33,32;,
 4;24,25,34,33;,
 4;27,26,35,36;,
 4;26,28,37,35;,
 4;28,29,38,37;,
 4;29,30,39,38;,
 4;30,31,40,39;,
 4;31,32,41,40;,
 4;32,33,42,41;,
 4;33,34,43,42;,
 4;36,35,44,45;,
 4;35,37,46,44;,
 4;37,38,47,46;,
 4;38,39,48,47;,
 4;39,40,49,48;,
 4;40,41,50,49;,
 4;41,42,51,50;,
 4;42,43,52,51;,
 4;45,44,53,54;,
 4;44,46,55,53;,
 4;46,47,56,55;,
 4;47,48,57,56;,
 4;48,49,58,57;,
 4;49,50,59,58;,
 4;50,51,60,59;,
 4;51,52,61,60;,
 4;54,53,62,63;,
 4;53,55,64,62;,
 4;55,56,65,64;,
 4;56,57,66,65;,
 4;57,58,67,66;,
 4;58,59,68,67;,
 4;59,60,69,68;,
 4;60,61,70,69;,
 3;63,62,71;,
 3;62,64,72;,
 3;64,65,73;,
 3;65,66,74;,
 3;66,67,75;,
 3;67,68,76;,
 3;68,69,77;,
 3;69,70,78;,
 3;79,80,81;,
 3;79,81,82;,
 3;79,82,83;,
 3;79,83,84;,
 3;79,84,85;,
 3;79,85,86;,
 3;79,86,87;,
 3;79,87,80;,
 4;80,88,89,81;,
 4;81,89,90,82;,
 4;82,90,91,83;,
 4;83,91,92,84;,
 4;84,92,93,85;,
 4;85,93,94,86;,
 4;86,94,95,87;,
 4;87,95,88,80;,
 4;88,96,97,89;,
 4;89,97,98,90;,
 4;90,98,99,91;,
 4;91,99,100,92;,
 4;92,100,101,93;,
 4;93,101,102,94;,
 4;94,102,103,95;,
 4;95,103,96,88;,
 4;96,104,105,97;,
 4;97,105,106,98;,
 4;98,106,107,99;,
 4;99,107,108,100;,
 4;100,108,109,101;,
 4;101,109,110,102;,
 4;102,110,111,103;,
 4;103,111,104,96;,
 4;104,112,113,105;,
 4;105,113,114,106;,
 4;106,114,115,107;,
 4;107,115,116,108;,
 4;108,116,117,109;,
 4;109,117,118,110;,
 4;110,118,119,111;,
 4;111,119,112,104;,
 4;112,120,121,113;,
 4;113,121,122,114;,
 4;114,122,123,115;,
 4;115,123,124,116;,
 4;116,124,125,117;,
 4;117,125,126,118;,
 4;118,126,127,119;,
 4;119,127,120,112;,
 4;120,128,129,121;,
 4;121,129,130,122;,
 4;122,130,131,123;,
 4;123,131,132,124;,
 4;124,132,133,125;,
 4;125,133,134,126;,
 4;126,134,135,127;,
 4;127,135,128,120;,
 4;128,136,137,129;,
 4;129,137,138,130;,
 4;130,138,139,131;,
 4;131,139,140,132;,
 4;132,140,141,133;,
 4;133,141,142,134;,
 4;134,142,143,135;,
 4;135,143,136,128;,
 4;136,144,145,137;,
 4;137,145,146,138;,
 4;138,146,147,139;,
 4;139,147,148,140;,
 4;140,148,149,141;,
 4;141,149,150,142;,
 4;142,150,151,143;,
 4;143,151,144,136;,
 4;144,152,153,145;,
 4;145,153,154,146;,
 4;146,154,155,147;,
 4;147,155,156,148;,
 4;148,156,157,149;,
 4;149,157,158,150;,
 4;150,158,159,151;,
 4;151,159,152,144;,
 3;152,160,153;,
 3;153,160,154;,
 3;154,160,155;,
 3;155,160,156;,
 3;156,160,157;,
 3;157,160,158;,
 3;158,160,159;,
 3;159,160,152;;
 
 MeshMaterialList {
  1;
  152;
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
   0.458400;0.620800;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
 MeshNormals {
  140;
  0.000001;1.000000;0.000000;,
  0.000001;0.924733;-0.380616;,
  0.269138;0.924733;-0.269136;,
  0.380617;0.924733;0.000000;,
  0.269138;0.924733;0.269136;,
  0.000001;0.924733;0.380616;,
  -0.269136;0.924733;0.269137;,
  -0.380616;0.924733;0.000000;,
  -0.269136;0.924733;-0.269137;,
  0.000001;0.709231;-0.704976;,
  0.498494;0.709231;-0.498493;,
  0.704977;0.709230;0.000000;,
  0.498494;0.709230;0.498493;,
  0.000001;0.709231;0.704976;,
  -0.498493;0.709231;0.498494;,
  -0.704976;0.709231;0.000000;,
  -0.498493;0.709231;-0.498495;,
  0.000001;0.384552;-0.923103;,
  0.652733;0.384552;-0.652732;,
  0.923104;0.384552;0.000000;,
  0.652733;0.384551;0.652733;,
  0.000000;0.384552;0.923103;,
  -0.652732;0.384553;0.652733;,
  -0.923103;0.384553;0.000000;,
  -0.652732;0.384553;-0.652733;,
  0.000002;0.000000;-1.000000;,
  0.707107;0.000000;-0.707106;,
  1.000000;0.000000;0.000000;,
  0.707107;0.000000;0.707107;,
  0.000000;0.000000;1.000000;,
  -0.707107;0.000000;0.707107;,
  -1.000000;0.000000;0.000000;,
  -0.707106;0.000000;-0.707107;,
  0.000001;-0.384552;-0.923103;,
  0.652733;-0.384552;-0.652732;,
  0.923104;-0.384552;-0.000000;,
  0.652733;-0.384551;0.652733;,
  0.000000;-0.384552;0.923103;,
  -0.652732;-0.384553;0.652733;,
  -0.923103;-0.384553;0.000000;,
  -0.652732;-0.384553;-0.652733;,
  0.000001;-0.709231;-0.704976;,
  0.498494;-0.709231;-0.498493;,
  0.704977;-0.709230;-0.000000;,
  0.498494;-0.709230;0.498493;,
  0.000001;-0.709231;0.704976;,
  -0.498493;-0.709231;0.498494;,
  -0.704976;-0.709231;0.000000;,
  -0.498493;-0.709231;-0.498495;,
  0.000001;-0.924736;-0.380609;,
  0.269133;-0.924736;-0.269131;,
  0.380610;-0.924736;-0.000000;,
  0.269133;-0.924736;0.269131;,
  0.000001;-0.924736;0.380609;,
  -0.269131;-0.924736;0.269132;,
  -0.380609;-0.924736;0.000000;,
  -0.269131;-0.924736;-0.269132;,
  0.000001;-1.000000;-0.000000;,
  -1.000000;0.000000;0.000000;,
  -0.985158;0.000000;-0.171649;,
  -0.982970;-0.138295;-0.121011;,
  -0.980805;-0.194991;0.000000;,
  -0.982970;-0.138295;0.121011;,
  -0.985158;0.000000;0.171649;,
  -0.982970;0.138297;0.121012;,
  -0.980804;0.194994;0.000000;,
  -0.982970;0.138297;-0.121012;,
  -0.820838;0.000000;-0.571161;,
  -0.802561;-0.446171;-0.396014;,
  -0.785329;-0.619079;0.000000;,
  -0.802561;-0.446171;0.396014;,
  -0.820838;0.000000;0.571161;,
  -0.802557;0.446176;0.396016;,
  -0.785322;0.619088;0.000000;,
  -0.802557;0.446176;-0.396016;,
  -0.499076;0.000000;-0.866558;,
  -0.472984;-0.655421;-0.588821;,
  -0.449509;-0.893276;0.000000;,
  -0.472983;-0.655421;0.588821;,
  -0.499076;0.000000;0.866558;,
  -0.472983;0.655422;0.588820;,
  -0.449507;0.893277;0.000000;,
  -0.472983;0.655422;-0.588820;,
  -0.228990;0.000000;-0.973429;,
  -0.214012;-0.724620;-0.655076;,
  -0.200849;-0.979622;0.000000;,
  -0.214012;-0.724620;0.655076;,
  -0.228990;0.000000;0.973429;,
  -0.214015;0.724620;0.655075;,
  -0.200855;0.979621;0.000000;,
  -0.214015;0.724620;-0.655075;,
  -0.054450;0.000000;-0.998516;,
  -0.050685;-0.740251;-0.670418;,
  -0.047391;-0.998876;0.000000;,
  -0.050685;-0.740251;0.670418;,
  -0.054450;0.000000;0.998516;,
  -0.050685;0.740251;0.670418;,
  -0.047391;0.998876;0.000000;,
  -0.050685;0.740251;-0.670418;,
  0.054451;0.000000;-0.998516;,
  0.050685;-0.740251;-0.670418;,
  0.047392;-0.998876;-0.000000;,
  0.050685;-0.740251;0.670418;,
  0.054451;0.000000;0.998516;,
  0.050683;0.740250;0.670418;,
  0.047386;0.998877;0.000000;,
  0.050683;0.740250;-0.670418;,
  0.228990;0.000000;-0.973429;,
  0.214014;-0.724620;-0.655075;,
  0.200854;-0.979621;-0.000000;,
  0.214014;-0.724620;0.655075;,
  0.228990;0.000000;0.973429;,
  0.214011;0.724620;0.655077;,
  0.200849;0.979622;0.000000;,
  0.214011;0.724620;-0.655077;,
  0.499058;0.000000;-0.866568;,
  0.472966;-0.655429;-0.588826;,
  0.449490;-0.893285;-0.000000;,
  0.472966;-0.655429;0.588826;,
  0.499058;0.000000;0.866568;,
  0.472968;0.655426;0.588827;,
  0.449497;0.893282;0.000000;,
  0.472968;0.655426;-0.588827;,
  0.820834;0.000000;-0.571167;,
  0.802555;-0.446178;-0.396018;,
  0.785320;-0.619089;-0.000000;,
  0.802555;-0.446178;0.396018;,
  0.820834;0.000000;0.571167;,
  0.802555;0.446177;0.396019;,
  0.785322;0.619087;0.000000;,
  0.802555;0.446177;-0.396019;,
  0.985160;0.000000;-0.171639;,
  0.982972;-0.138287;-0.121004;,
  0.980807;-0.194980;-0.000000;,
  0.982972;-0.138287;0.121004;,
  0.985160;0.000000;0.171639;,
  0.982972;0.138289;0.121005;,
  0.980807;0.194984;0.000000;,
  0.982972;0.138289;-0.121005;,
  1.000000;0.000000;0.000000;;
  152;
  3;0,2,1;,
  3;0,3,2;,
  3;0,4,3;,
  3;0,5,4;,
  3;0,6,5;,
  3;0,7,6;,
  3;0,8,7;,
  3;0,1,8;,
  4;1,2,10,9;,
  4;2,3,11,10;,
  4;3,4,12,11;,
  4;4,5,13,12;,
  4;5,6,14,13;,
  4;6,7,15,14;,
  4;7,8,16,15;,
  4;8,1,9,16;,
  4;9,10,18,17;,
  4;10,11,19,18;,
  4;11,12,20,19;,
  4;12,13,21,20;,
  4;13,14,22,21;,
  4;14,15,23,22;,
  4;15,16,24,23;,
  4;16,9,17,24;,
  4;17,18,26,25;,
  4;18,19,27,26;,
  4;19,20,28,27;,
  4;20,21,29,28;,
  4;21,22,30,29;,
  4;22,23,31,30;,
  4;23,24,32,31;,
  4;24,17,25,32;,
  4;25,26,34,33;,
  4;26,27,35,34;,
  4;27,28,36,35;,
  4;28,29,37,36;,
  4;29,30,38,37;,
  4;30,31,39,38;,
  4;31,32,40,39;,
  4;32,25,33,40;,
  4;33,34,42,41;,
  4;34,35,43,42;,
  4;35,36,44,43;,
  4;36,37,45,44;,
  4;37,38,46,45;,
  4;38,39,47,46;,
  4;39,40,48,47;,
  4;40,33,41,48;,
  4;41,42,50,49;,
  4;42,43,51,50;,
  4;43,44,52,51;,
  4;44,45,53,52;,
  4;45,46,54,53;,
  4;46,47,55,54;,
  4;47,48,56,55;,
  4;48,41,49,56;,
  3;49,50,57;,
  3;50,51,57;,
  3;51,52,57;,
  3;52,53,57;,
  3;53,54,57;,
  3;54,55,57;,
  3;55,56,57;,
  3;56,49,57;,
  3;58,59,60;,
  3;58,60,61;,
  3;58,61,62;,
  3;58,62,63;,
  3;58,63,64;,
  3;58,64,65;,
  3;58,65,66;,
  3;58,66,59;,
  4;59,67,68,60;,
  4;60,68,69,61;,
  4;61,69,70,62;,
  4;62,70,71,63;,
  4;63,71,72,64;,
  4;64,72,73,65;,
  4;65,73,74,66;,
  4;66,74,67,59;,
  4;67,75,76,68;,
  4;68,76,77,69;,
  4;69,77,78,70;,
  4;70,78,79,71;,
  4;71,79,80,72;,
  4;72,80,81,73;,
  4;73,81,82,74;,
  4;74,82,75,67;,
  4;75,83,84,76;,
  4;76,84,85,77;,
  4;77,85,86,78;,
  4;78,86,87,79;,
  4;79,87,88,80;,
  4;80,88,89,81;,
  4;81,89,90,82;,
  4;82,90,83,75;,
  4;83,91,92,84;,
  4;84,92,93,85;,
  4;85,93,94,86;,
  4;86,94,95,87;,
  4;87,95,96,88;,
  4;88,96,97,89;,
  4;89,97,98,90;,
  4;90,98,91,83;,
  4;91,99,100,92;,
  4;92,100,101,93;,
  4;93,101,102,94;,
  4;94,102,103,95;,
  4;95,103,104,96;,
  4;96,104,105,97;,
  4;97,105,106,98;,
  4;98,106,99,91;,
  4;99,107,108,100;,
  4;100,108,109,101;,
  4;101,109,110,102;,
  4;102,110,111,103;,
  4;103,111,112,104;,
  4;104,112,113,105;,
  4;105,113,114,106;,
  4;106,114,107,99;,
  4;107,115,116,108;,
  4;108,116,117,109;,
  4;109,117,118,110;,
  4;110,118,119,111;,
  4;111,119,120,112;,
  4;112,120,121,113;,
  4;113,121,122,114;,
  4;114,122,115,107;,
  4;115,123,124,116;,
  4;116,124,125,117;,
  4;117,125,126,118;,
  4;118,126,127,119;,
  4;119,127,128,120;,
  4;120,128,129,121;,
  4;121,129,130,122;,
  4;122,130,123,115;,
  4;123,131,132,124;,
  4;124,132,133,125;,
  4;125,133,134,126;,
  4;126,134,135,127;,
  4;127,135,136,128;,
  4;128,136,137,129;,
  4;129,137,138,130;,
  4;130,138,131,123;,
  3;131,139,132;,
  3;132,139,133;,
  3;133,139,134;,
  3;134,139,135;,
  3;135,139,136;,
  3;136,139,137;,
  3;137,139,138;,
  3;138,139,131;;
 }
 MeshTextureCoords {
  161;
  0.062500;0.000000;,
  0.125000;0.125000;,
  0.000000;0.125000;,
  0.187500;0.000000;,
  0.250000;0.125000;,
  0.312500;0.000000;,
  0.375000;0.125000;,
  0.437500;0.000000;,
  0.500000;0.125000;,
  0.562500;0.000000;,
  0.625000;0.125000;,
  0.687500;0.000000;,
  0.750000;0.125000;,
  0.812500;0.000000;,
  0.875000;0.125000;,
  0.937500;0.000000;,
  1.000000;0.125000;,
  0.125000;0.250000;,
  0.000000;0.250000;,
  0.250000;0.250000;,
  0.375000;0.250000;,
  0.500000;0.250000;,
  0.625000;0.250000;,
  0.750000;0.250000;,
  0.875000;0.250000;,
  1.000000;0.250000;,
  0.125000;0.375000;,
  0.000000;0.375000;,
  0.250000;0.375000;,
  0.375000;0.375000;,
  0.500000;0.375000;,
  0.625000;0.375000;,
  0.750000;0.375000;,
  0.875000;0.375000;,
  1.000000;0.375000;,
  0.125000;0.500000;,
  0.000000;0.500000;,
  0.250000;0.500000;,
  0.375000;0.500000;,
  0.500000;0.500000;,
  0.625000;0.500000;,
  0.750000;0.500000;,
  0.875000;0.500000;,
  1.000000;0.500000;,
  0.125000;0.625000;,
  0.000000;0.625000;,
  0.250000;0.625000;,
  0.375000;0.625000;,
  0.500000;0.625000;,
  0.625000;0.625000;,
  0.750000;0.625000;,
  0.875000;0.625000;,
  1.000000;0.625000;,
  0.125000;0.750000;,
  0.000000;0.750000;,
  0.250000;0.750000;,
  0.375000;0.750000;,
  0.500000;0.750000;,
  0.625000;0.750000;,
  0.750000;0.750000;,
  0.875000;0.750000;,
  1.000000;0.750000;,
  0.125000;0.875000;,
  0.000000;0.875000;,
  0.250000;0.875000;,
  0.375000;0.875000;,
  0.500000;0.875000;,
  0.625000;0.875000;,
  0.750000;0.875000;,
  0.875000;0.875000;,
  1.000000;0.875000;,
  0.062500;1.000000;,
  0.187500;1.000000;,
  0.312500;1.000000;,
  0.437500;1.000000;,
  0.562500;1.000000;,
  0.687500;1.000000;,
  0.812500;1.000000;,
  0.937500;1.000000;,
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
