#ifndef INCLUDED_CUSTOMSTRUCT_H_
#define INCLUDED_CUSTOMSTRUCT_H_

//RGBA�F�����\����
struct RGBAColor {
	int Red;
	int Green;
	int Blue;
	float Alpha;
};

//���_�\����
struct CUSTOMVERTEX {
	XMFLOAT4	v4Pos;
	XMFLOAT4	v4Color;
};

//�V�F�[�_�萔�\����
struct CBNeverChanges {
	XMMATRIX mView;
};

//�e�N�X�`���G�\����
struct TEX_PICTURE {
	ID3D11ShaderResourceView	*pSRViewTexture;
	D3D11_TEXTURE2D_DESC		tdDesc;
	int							nWidth, nHeight;
};
#endif // !INCLUDED_RGBACOLOR_H_
