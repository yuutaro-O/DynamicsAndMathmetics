#ifndef INCLUDED_CUSTOMSTRUCT_H_
#define INCLUDED_CUSTOMSTRUCT_H_

//RGBA色合い構造体
struct RGBAColor {
	int Red;
	int Green;
	int Blue;
	float Alpha;
};

//頂点構造体
struct CUSTOMVERTEX {
	XMFLOAT4	v4Pos;
	XMFLOAT4	v4Color;
};

//シェーダ定数構造体
struct CBNeverChanges {
	XMMATRIX mView;
};

//テクスチャ絵構造体
struct TEX_PICTURE {
	ID3D11ShaderResourceView	*pSRViewTexture;
	D3D11_TEXTURE2D_DESC		tdDesc;
	int							nWidth, nHeight;
};
#endif // !INCLUDED_RGBACOLOR_H_
