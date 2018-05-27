#define DIRECTINPUT_VERSION 0x0800
#include <stdio.h>
#include <crtdbg.h>
#include "main.h"
#include "SceneTransition.h"
#include "MousePosition.h"
#include "CTitle.h"
#include "CRenderer.h"
#include <imgui.h>
#include "imgui_impl_dx9.h"
#include "imconfig.h"
#include "imgui_internal.h"

//-----------------------------------------------------------------------------------------
//マクロ定義
//-----------------------------------------------------------------------------------------
#define CLASS_NAME      "Framework"                  //ウィンドウクラスの名前
#define WINDOW_NAME     "Framework"        //ウィンドウの名前

#define TIMER_INTERVAL (1000/60)

int g_nCountFPS = 0;
bool g_bExitGame = false;

//クラス型
CSceneTransition *g_SceneTra;

//-----------------------------------------------------------------------------------------
//プロトタイプ宣言
//-----------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//ファンクション関数
void WindowClassRegistration(HINSTANCE hInstance);   //ウィンドウクラスの登録
void MakeTheWindow(HINSTANCE hInstance, int nCmdShow,HWND *phWnd); //ウィンドウ作成
void MainLoop(MSG *msg, const HWND& hWnd);  //メインループ

//-----------------------------------------------------------------------------------------
//メイン関数
// (APIENTRY : Win32API関数を呼び出す時の規約[WINAPIでも良い])
// hInstance     : このアプリケーションのインスタンスハンドル(識別子)
// hPrevInstance : 常にnullptr(16bit時代の名残り)
// lpCmdLine     : コマンドラインから受け取った引数へのポインタ
// nCmdShow      : アプリケーションの初期ウインドウ表示方法の指定
//-----------------------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HWND hWnd;

	//warningを表示させないため、なんも役が立たない
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WindowClassRegistration(hInstance);				//ウィンドウクラスの登録
	MakeTheWindow(hInstance, nCmdShow, &hWnd);		//ウィンドウ作成

	g_SceneTra = new CSceneTransition();
	g_SceneTra->Init(hInstance, hWnd, 1);

	MainLoop(&msg, hWnd);			//メインループ

	g_SceneTra->Uninit();			//解放
	delete g_SceneTra;

	_CrtDumpMemoryLeaks();			//メモリリーク検出

	return (int)msg.wParam;
}

void WindowClassRegistration(HINSTANCE hInstance)
{
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),                     //WNDCLASSEXのメモリサイズを指定
		CS_CLASSDC,                             //表示するウィンドウのスタイルを指定
		WndProc,								//ウィンドウプロシージャのアドレス(関数名)を指定
		0L,										//通常は使用しないので"0"を指定
		0L,										//通常は使用しないので"0"を指定
		hInstance,								//WinMainのパラメータのインスタンスハンドルを指定
		nullptr,									//使用するアイコンを指定(Windowsが持っているアイコンを使う)
		LoadCursor(nullptr,IDC_ARROW),				//マウスカーソルを指定
		(HBRUSH)(COLOR_WINDOW + 1),				//ウィンドウのクライアント領域の背景色を設定
		nullptr,									//Windowにつけるメニューを設定
		CLASS_NAME,								//ウィンドウクラスの名前
		nullptr									//小さいアイコンが設定された場合の情報を記述
	};

	//ウィンドウクラスの登録(オリジナルのウィンドウの名前の登録)
	RegisterClassEx(&wcex);
}

void MakeTheWindow(HINSTANCE hInstance, int nCmdShow, HWND *phWnd)
{
	RECT cr = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	AdjustWindowRect(&cr, WS_OVERLAPPEDWINDOW ^ (WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX), FALSE);

	RECT dr;
	GetWindowRect(GetDesktopWindow(), &dr);

	int dw = dr.right - dr.left;
	int dh = dr.bottom - dr.top;

	int ww = cr.right - cr.left;
	int wh = cr.bottom - cr.top;

	int wx = ww > dw ? 0 : (dw - ww) / 2;
	int wy = wh > dh ? 0 : (dh - wh) / 2;

	//ウィンドウを作成
	*phWnd = CreateWindowEx(0,                    //拡張ウィンドウスタイル
		CLASS_NAME,								//ウィンドウクラスの名前
		WINDOW_NAME,							//ウィンドウの名前
		WS_OVERLAPPEDWINDOW ^ (WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX),					//ウィンドウスタイル
		wx,							            //ウィンドウの左上X座標
		wy,							            //ウィンドウの左上Y座標
		ww,							            //ウィンドウの幅
		wh,							            //ウィンドウの高さ
		nullptr,									//親ウィンドウのハンドル
		nullptr,									//メニューハンドルまたは子ウィンドウID
		hInstance,								//インスタンスハンドル
		nullptr);									//ウィンドウ作成データ


	//ウィンドウの表示
	ShowWindow(*phWnd, nCmdShow);					//指定されたウインドウの表示
	UpdateWindow(*phWnd);							//ウィンドウの状態直ちに返す
}

void MainLoop(MSG *msg, const HWND& hWnd)
{
	//分解能の設定
	timeBeginPeriod(1);

	//各カウンターの初期化
	DWORD dwFrameCount = 0;
	DWORD dwCurrentTime = 0;
	DWORD dwExecLastTime = timeGetTime();                  //現在の時刻をミリ秒単位
	DWORD dwFPSLastTime = dwExecLastTime;

	//メッセージループ
	for (;;)
	{
		if (PeekMessage(msg, nullptr, 0, 0, PM_REMOVE) != 0)
		{
			//Windowsの処理
			if (msg->message == WM_QUIT) break;
			else
			{
				TranslateMessage(msg);               //呼び出し側スレッドのメッセージキューか
				DispatchMessage(msg);                //メッセージの翻訳と送出
			}
		}

		//ゲームの処理
		else
		{
			dwCurrentTime = timeGetTime();

			if (dwCurrentTime - dwFPSLastTime >= 500)
			{
				g_nCountFPS = ((dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime));
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}

			//1フレームの時間が経ったら一回ゲームループ
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
				dwExecLastTime = dwCurrentTime;

				//DirectXの処理
				CMousePosition::GetMousePosFromWindow(hWnd);                       //マウスの座標保存
				CTitle::SetWnd(hWnd);
				ImGui_ImplDX9_NewFrame();				//imgui開始
				g_SceneTra->Update();
				g_SceneTra->Draw();
				dwFrameCount++;
			}
		}
	}

	//分解能を戻す
	timeEndPeriod(1);
}

//-----------------------------------------------------------------------------------------
//ウィンドウプロシージャ
//(CALLBACK:Win32API関数を呼び出す時の規約)
//hWnd   :ウィンドウのハンドル
//uMsg   :メッセージの識別子
//wParam :メッセージの最初のパラメータ
//LParam :メッセージの2番目のパラメータ
//-----------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		return true;
	}

	static float x = 0;
	static float y = 0;

	static char buf[256];
	const RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);							//"WM_QUIT"メッセージを返す
		break;
	case WM_CREATE:
		break;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_COMMAND:
		break;
	case WM_TIMER:
		break;
	case WM_PAINT:
		break;
	case WM_KEYDOWN:
		break;
	case WM_CLOSE:
		break;
	case WM_SIZE:
		if (CRenderer::GetDevice() != nullptr && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			CRenderer::GetDpp().BackBufferWidth = LOWORD(lParam);
			CRenderer::GetDpp().BackBufferHeight = HIWORD(lParam);
			HRESULT hr = CRenderer::GetDevice()->Reset(&CRenderer::GetDpp());
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		return 0;
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);  //既定の処理を提供(ウィンドウの基本の動きを提供する(これはないと危険！))
}