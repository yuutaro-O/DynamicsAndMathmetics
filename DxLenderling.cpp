#ifndef INCLUDED_DXLENDERLING_H_
#include "DxLenderling.h"
#endif
#ifndef INCLUDED_USERDRAWING_H_
#include "UserDrawing.h"
#endif

#define MAX_BUFFER_VERTEX	20000	//最大バッファ頂点数

//リンクライブラリ
#pragma comment (lib, "d3d11.lib")	//D3D11ライブラリ
#pragma comment (lib, "d3dx11.lib")

//セーフリリースマクロ
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)	{if(p) {(p)->Release(); (p) = NULL;}}
#endif

//グローバル変数
UINT g_nClientWidth;	//描画領域の横幅
UINT g_nClientHeight;	//描画領域の高さ

HWND g_hWnd;			//ウィンドウハンドル

ID3D11Device *g_pd3dDevice;	//デバイス
/*スワップチェイン
レンダリングを制御するための連続したバッファの事*/
IDXGISwapChain		*g_pSwapChain;	//DXGIスワップチェイン
ID3D11DeviceContext	*g_pImmediateContext;	//デバイスコンテキスト
/*ラスタライズ
スクリーン座標で表されたポリゴンをプクセルの集まりに分割する処理
それを担当するユニットをラスタライザという
*/
ID3D11RasterizerState	*g_pRS;				//ラスタライザ
/*レンダリングターゲット
読んで字のごとく、レンダリング対象の領域の事*/
ID3D11RenderTargetView	*g_pRTV;				//レンダリングターゲット
/*フィーチャーレベル
シェーダーモデルのバージョンを変えたりするときに使う*/
D3D_FEATURE_LEVEL		*g_FeatureLevel;	//フィーチャーレベル

ID3D11Buffer			*g_pD3D11VertexBuffer;	//頂点バッファ
/*アルファブレンド
α値を用いて2つの画像を合成する処理。
半透明を表現するときなどに使う*/
ID3D11BlendState		*g_pbsAlphaBlend;		//アルファブレンド
/*頂点シェーダ
描画する形状上の頂点ごとに処理を行うシェーダの事。
３Ｄは基本、ポリゴンという三角形で表現されるため、
頂点の位置が変わると描画の様子も変えられるという原理。
グラデーションなどをした場合、頂点間の色合いは補完処理される*/
ID3D11VertexShader		*g_pVertexShader;		//頂点シェーダ
/*ピクセルシェーダ(fragment shaderとも)
形状ごとではなく、画面に表示するピクセル毎に属性を変更できる
シェーダの事。
詳細な表現が出来る反面、頂点シェーダ(Vertex shader)に比べると処理が重いという欠点がある*/
ID3D11PixelShader		*g_pPixelShader;		//ピクセルシェーダ
/*シェーダ入力レイアウト
頂点の情報とシェーダの情報を登録する仲介インターフェース。*/
ID3D11InputLayout		*g_pInputLayout;		//シェーダ入力レイアウト
/*サンプラーステート
描画に使用するテクスチャやフィルタなどを保存した構造体。
*/
ID3D11SamplerState		*g_pSamplerState;		//サンプラステート

ID3D11Buffer			*g_pCBNeverChanges = NULL;

//描画頂点バッファ
CUSTOMVERTEX g_cvVertices[MAX_BUFFER_VERTEX];
int	g_nVertexNum = 0;
ID3D11ShaderResourceView	*g_pNowTexture = NULL;

//Direct3Dの初期化
HRESULT InitD3D(void) {
	HRESULT hr = S_OK;
	D3D_FEATURE_LEVEL FeatureLevelsRequested[6] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	UINT numLevelsRequested = 6;
	D3D_FEATURE_LEVEL FeatureLevelsSupported;

	//デバイス作成
	hr = D3D11CreateDevice(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		FeatureLevelsRequested,
		numLevelsRequested,
		D3D11_SDK_VERSION,
		&g_pd3dDevice,
		&FeatureLevelsSupported,
		&g_pImmediateContext);

	if (FAILED(hr)) {
		return hr;
	}

	//ファクトリの取得
	IDXGIDevice * pDXGIDevice;
	hr = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);
	IDXGIFactory * pIDXGIFactory;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);

	//スワップチェインの作成
	DXGI_SWAP_CHAIN_DESC	sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = g_nClientWidth;
	sd.BufferDesc.Height = g_nClientHeight;
	/*4成分で構成された、32ビット符号なし整数フォーマット*/
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	hr = pIDXGIFactory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);

	pDXGIDevice->Release();
	pDXGIAdapter->Release();
	pIDXGIFactory->Release();

	if (FAILED(hr)) {
		return hr;
	}

	//レンダリングターゲットの生成
	ID3D11Texture2D	*pBackBuffer = NULL;
	D3D11_TEXTURE2D_DESC	BackBufferSurfaceDesc;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr)) {
		MessageBox(NULL, _T("Can't get backbuffer."), _T("Error"), MB_OK);
	}
	pBackBuffer->GetDesc(&BackBufferSurfaceDesc);
	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRTV);
	SAFE_RELEASE(pBackBuffer);
	if (FAILED(hr)) {
		MessageBox(NULL, _T("Can't create render target view"), _T("Error"), MB_OK);
		return hr;
	}

	g_pImmediateContext->OMSetRenderTargets(1, &g_pRTV, NULL);

	//ラスタライザの設定
	D3D11_RASTERIZER_DESC drd;
	ZeroMemory(&drd, sizeof(drd));
	drd.FillMode = D3D11_FILL_SOLID;
	drd.CullMode = D3D11_CULL_NONE;
	drd.FrontCounterClockwise = FALSE;
	drd.DepthClipEnable = TRUE;
	hr = g_pd3dDevice->CreateRasterizerState(&drd, &g_pRS);
	if (FAILED(hr)) {
		MessageBox(NULL, _T("Can't create rasterizer state."), _T("Error"), MB_OK);
		return hr;
	}
	g_pImmediateContext->RSSetState(g_pRS);

	//ビューポートの設定
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)g_nClientWidth;
	vp.Height = (FLOAT)g_nClientHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	g_pImmediateContext->RSSetViewports(1, &vp);

	return S_OK;
}

HRESULT MakeShaders(void) {
	HRESULT hr;

	ID3DBlob* pVertexShaderBuffer = NULL;
	ID3DBlob* pPixelShaderBuffer = NULL;
	ID3DBlob* pError = NULL;
	
	DWORD dwShaderFlags = 0;

#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	hr = D3DX11CompileFromFile(_T("Basic_2D_Geom.fx"), NULL, NULL, "VS", "vs_4_0_level_9_1",
		dwShaderFlags, 0, NULL, &pVertexShaderBuffer, &pError, NULL);
	if (FAILED(hr)) {
		MessageBox(NULL, _T("Can't open Basic_2D_Geom.fx"), _T("Error"), MB_OK);
		SAFE_RELEASE(pError);
		return hr;
	}
	hr = D3DX11CompileFromFile(_T("Basic_2D_Geom.fx"), NULL, NULL, "PS", "ps_4_0_level_9_1",
								dwShaderFlags, 0, NULL, &pPixelShaderBuffer, &pError, NULL);
	if (FAILED(hr)) {
		SAFE_RELEASE(pVertexShaderBuffer);
		SAFE_RELEASE(pError);
		return hr;
	}
	SAFE_RELEASE(pError);

	//VertexShader作成
	hr = g_pd3dDevice->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),
											pVertexShaderBuffer->GetBufferSize(),
											NULL, &g_pVertexShader);
	if (FAILED(hr)) {
		SAFE_RELEASE(pVertexShaderBuffer);
		SAFE_RELEASE(pPixelShaderBuffer);
		return hr;
	}
	SAFE_RELEASE(pError);

	//PixelShader作成
	hr = g_pd3dDevice->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(),
											pPixelShaderBuffer->GetBufferSize(),
											NULL, &g_pPixelShader);
	if (FAILED(hr)) {
		SAFE_RELEASE(pVertexShaderBuffer);
		SAFE_RELEASE(pPixelShaderBuffer);
		return hr;
	}

	//入力バッファの入力形式
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",	0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,16,D3D11_INPUT_PER_VERTEX_DATA,0},
		//{"TEXTURE",	0,DXGI_FORMAT_R32G32_FLOAT		,0,32,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	UINT numElements = ARRAYSIZE(layout);
	//入力バッファの入力形式作成
	hr = g_pd3dDevice->CreateInputLayout(layout, numElements,
										pVertexShaderBuffer->GetBufferPointer(),
										pVertexShaderBuffer->GetBufferSize(),
										&g_pInputLayout);
	SAFE_RELEASE(pVertexShaderBuffer);
	SAFE_RELEASE(pPixelShaderBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	//シェーダ定数バッファ作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBNeverChanges);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pCBNeverChanges);
	if (FAILED(hr)) return hr;

	//変換表列
	CBNeverChanges	cbNeverChanges;
	XMMATRIX		mScreen;
	mScreen		= XMMatrixIdentity();
	mScreen._11	= 2.0f / g_nClientWidth;
	mScreen._22	= -2.0f / g_nClientHeight;
	mScreen._41	= -1.0f;
	mScreen._42	= 1.0f;
	cbNeverChanges.mView = XMMatrixTranspose(mScreen);
	g_pImmediateContext->UpdateSubresource(g_pCBNeverChanges, 0, NULL, &cbNeverChanges, 0, 0);

	return S_OK;
}

//描画モードオブジェクト初期化
int InitDrawModes(void) {
	HRESULT	hr;

	//ブレンドステート
	D3D11_BLEND_DESC BlendDesc;
	BlendDesc.AlphaToCoverageEnable					= FALSE;
	BlendDesc.IndependentBlendEnable				= FALSE;
	BlendDesc.RenderTarget[0].BlendEnable			= TRUE;
	BlendDesc.RenderTarget[0].SrcBlend				= D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha		= D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pd3dDevice->CreateBlendState(&BlendDesc, &g_pbsAlphaBlend);
	if (FAILED(hr)) {
		return hr;
	}

	//サンプラ
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory(&samDesc, sizeof(samDesc));
	samDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.ComparisonFunc	= D3D11_COMPARISON_ALWAYS;
	samDesc.MaxLOD			= D3D11_FLOAT32_MAX;
	hr = g_pd3dDevice->CreateSamplerState(&samDesc, &g_pSamplerState);
	if (FAILED(hr)) {
		return hr;
	}
	return S_OK;
}

//ジオメトリの初期化
HRESULT InitGeometry(void) {
	HRESULT hr = S_OK;

	//頂点バッファ作成
	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	BufferDesc.ByteWidth		= sizeof(CUSTOMVERTEX) * MAX_BUFFER_VERTEX;
	BufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags		= 0;

	D3D11_SUBRESOURCE_DATA SubResourceData;
	SubResourceData.pSysMem = g_cvVertices;
	SubResourceData.SysMemPitch = 0;
	SubResourceData.SysMemSlicePitch = 0;
	hr = g_pd3dDevice->CreateBuffer(&BufferDesc, &SubResourceData, &g_pD3D11VertexBuffer);
	if (FAILED(hr)) {
		return hr;
	}
	return S_OK;
}
/*終了処理
プログラムの最後に必ず呼び出そう*/
int Cleanup(void) {
	//SAFE_RELEASE(g_tBall.pSRViewTexture);
	//SAFE_RELEASE(g_tBack.pSRViewTexture);
	SAFE_RELEASE(g_pD3D11VertexBuffer);

	SAFE_RELEASE(g_pSamplerState);
	SAFE_RELEASE(g_pbsAlphaBlend);
	SAFE_RELEASE(g_pInputLayout);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pCBNeverChanges);

	SAFE_RELEASE(g_pRS);

	//ステータスをクリア
	if (g_pImmediateContext) {
		g_pImmediateContext->ClearState();
		g_pImmediateContext->Flush();
	}
	SAFE_RELEASE(g_pRTV);	//レンダリングターゲット

	//スワップチェーン
	if (g_pSwapChain != NULL) {
		g_pSwapChain->SetFullscreenState(FALSE, 0);
	}
	SAFE_RELEASE(g_pSwapChain);

	SAFE_RELEASE(g_pSwapChain);
	SAFE_RELEASE(g_pImmediateContext);	//テバイスコンテキスト
	SAFE_RELEASE(g_pd3dDevice);			//デバイス

	return 0;
	
}

//ウィンドウプロシージャ
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//絵の描画待ち行列フラッシュ
/*D3D11_MAPPED_SUBRESOURCE（サブリソース）とは？
　所謂バッファの事。DX11では、画面の情報を
  サブセット（プログラム全体の中の、一部分の情報を指す）
  として持っておく。このサブセットの集合体が
  サブリソースである。*/
int FlushDrawingPictures(void) {
	HRESULT hr;

	if (g_nVertexNum > 0) {
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hr = g_pImmediateContext->Map(g_pD3D11VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (SUCCEEDED(hr)) {
			CopyMemory(mappedResource.pData, &(g_cvVertices[0]), sizeof(CUSTOMVERTEX) * g_nVertexNum);
			g_pImmediateContext->Unmap(g_pD3D11VertexBuffer, 0);
		}
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pNowTexture);
		g_pImmediateContext->Draw(g_nVertexNum, 0);
	}
	g_nVertexNum = 0;
	g_pNowTexture = NULL;

	return 0;
}

int DrawPoints(int x, int y, RGBAColor color) {
	g_cvVertices[g_nVertexNum].v4Pos = XMFLOAT4((float)x, (float)y, 0.0f, 1.0f);
	g_cvVertices[g_nVertexNum].v4Color = XMFLOAT4(color.Red / 255.0f, color.Green / 255.0f, color.Blue / 255.0f, 1.0f);
	g_nVertexNum++;

	return 0;
}

int DrawPoints(int x, int y, int nRed, int nGreen, int nBlue) {
	g_cvVertices[g_nVertexNum].v4Pos = XMFLOAT4((float)x, (float)y, 0.0f, 1.0f);
	g_cvVertices[g_nVertexNum].v4Color = XMFLOAT4(nRed / 255.0f, nGreen / 255.0f, nBlue / 255.0f, 1.0f);
	g_nVertexNum++;

	return 0;
}

HRESULT Render(void) {
	//int i,j;
	//画面クリア
	XMFLOAT4 v4Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	g_pImmediateContext->ClearRenderTargetView(g_pRTV, (float *)&v4Color);

	//サンプラ・ラスタライザセット
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerState);
	g_pImmediateContext->RSSetState(g_pRS);

	//描画設定
	UINT nStrides = sizeof(CUSTOMVERTEX);
	UINT nOffsets = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pD3D11VertexBuffer, &nStrides, &nOffsets);
	//g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	g_pImmediateContext->IASetInputLayout(g_pInputLayout);

	//シェーダ設定
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBNeverChanges);
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);

	//描画
	g_pImmediateContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	UserDrawing();
	//RenderScanLine();

	//表示
	FlushDrawingPictures();

	return S_OK;
}

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, int) {
	LARGE_INTEGER nNowtime, nLastTime;	//現在と一つ前の時刻
	LARGE_INTEGER nTimeFreq;			//時間単位

	//画面サイズ
	g_nClientWidth = VIEW_WIDTH;		//幅
	g_nClientHeight = VIEW_HEIGHT;		//高さ

	//Register the Window class
	WNDCLASSEX wc = { sizeof(WNDCLASSEX),CS_CLASSDC,MsgProc,0L,0L,
					GetModuleHandle(NULL),NULL,NULL,NULL,NULL,
					_T("D3D Sample"),NULL };
	RegisterClassEx(&wc);

	RECT rcRect;
	SetRect(&rcRect, 0, 0, g_nClientWidth, g_nClientHeight);
	AdjustWindowRect(&rcRect, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(_T("D3D Sample"), _T("Render_1_1"),
							WS_OVERLAPPEDWINDOW, 100, 20, rcRect.right - rcRect.left, rcRect.bottom - rcRect.top,
							GetDesktopWindow(), NULL, wc.hInstance, NULL);
	//Initialize Direct3D
	if (SUCCEEDED(InitD3D()) && SUCCEEDED(MakeShaders())) {
		//Create the shaders
		if (SUCCEEDED(InitDrawModes())) {
			if (SUCCEEDED(InitGeometry())) { //ジオメトリ作成
				//show the window
				ShowWindow(g_hWnd, SW_SHOWDEFAULT);
				UpdateWindow(g_hWnd);

				//InitCharacter()// キャラクタ初期化

				QueryPerformanceFrequency(&nTimeFreq);	//時間単位
				QueryPerformanceCounter(&nLastTime);	//１フレーム前時刻の初期化

				//Enter the Message loop
				MSG msg;
				ZeroMemory(&msg, sizeof(msg));
				while (msg.message != WM_QUIT) {
					Render();
					do {
						if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}
						QueryPerformanceCounter(&nNowtime);
					} while (((nNowtime.QuadPart - nLastTime.QuadPart) < (nTimeFreq.QuadPart / 90)) && (msg.message != WM_QUIT));
					while (((nNowtime.QuadPart - nLastTime.QuadPart) < (nTimeFreq.QuadPart / 60)) && (msg.message != WM_QUIT)) {
						QueryPerformanceCounter(&nNowtime);
					}
					nLastTime = nNowtime;
					g_pSwapChain->Present(0, 0); //表示

				}
			}
		}
	}
	Cleanup();
	UnregisterClass(_T("D3D Sample"), wc.hInstance);
	return 0;
}