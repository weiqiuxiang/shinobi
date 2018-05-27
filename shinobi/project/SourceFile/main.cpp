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
//�}�N����`
//-----------------------------------------------------------------------------------------
#define CLASS_NAME      "Framework"                  //�E�B���h�E�N���X�̖��O
#define WINDOW_NAME     "Framework"        //�E�B���h�E�̖��O

#define TIMER_INTERVAL (1000/60)

int g_nCountFPS = 0;
bool g_bExitGame = false;

//�N���X�^
CSceneTransition *g_SceneTra;

//-----------------------------------------------------------------------------------------
//�v���g�^�C�v�錾
//-----------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//�t�@���N�V�����֐�
void WindowClassRegistration(HINSTANCE hInstance);   //�E�B���h�E�N���X�̓o�^
void MakeTheWindow(HINSTANCE hInstance, int nCmdShow,HWND *phWnd); //�E�B���h�E�쐬
void MainLoop(MSG *msg, const HWND& hWnd);  //���C�����[�v

//-----------------------------------------------------------------------------------------
//���C���֐�
// (APIENTRY : Win32API�֐����Ăяo�����̋K��[WINAPI�ł��ǂ�])
// hInstance     : ���̃A�v���P�[�V�����̃C���X�^���X�n���h��(���ʎq)
// hPrevInstance : ���nullptr(16bit����̖��c��)
// lpCmdLine     : �R�}���h���C������󂯎���������ւ̃|�C���^
// nCmdShow      : �A�v���P�[�V�����̏����E�C���h�E�\�����@�̎w��
//-----------------------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HWND hWnd;

	//warning��\�������Ȃ����߁A�Ȃ�����������Ȃ�
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WindowClassRegistration(hInstance);				//�E�B���h�E�N���X�̓o�^
	MakeTheWindow(hInstance, nCmdShow, &hWnd);		//�E�B���h�E�쐬

	g_SceneTra = new CSceneTransition();
	g_SceneTra->Init(hInstance, hWnd, 1);

	MainLoop(&msg, hWnd);			//���C�����[�v

	g_SceneTra->Uninit();			//���
	delete g_SceneTra;

	_CrtDumpMemoryLeaks();			//���������[�N���o

	return (int)msg.wParam;
}

void WindowClassRegistration(HINSTANCE hInstance)
{
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),                     //WNDCLASSEX�̃������T�C�Y���w��
		CS_CLASSDC,                             //�\������E�B���h�E�̃X�^�C�����w��
		WndProc,								//�E�B���h�E�v���V�[�W���̃A�h���X(�֐���)���w��
		0L,										//�ʏ�͎g�p���Ȃ��̂�"0"���w��
		0L,										//�ʏ�͎g�p���Ȃ��̂�"0"���w��
		hInstance,								//WinMain�̃p�����[�^�̃C���X�^���X�n���h�����w��
		nullptr,									//�g�p����A�C�R�����w��(Windows�������Ă���A�C�R�����g��)
		LoadCursor(nullptr,IDC_ARROW),				//�}�E�X�J�[�\�����w��
		(HBRUSH)(COLOR_WINDOW + 1),				//�E�B���h�E�̃N���C�A���g�̈�̔w�i�F��ݒ�
		nullptr,									//Window�ɂ��郁�j���[��ݒ�
		CLASS_NAME,								//�E�B���h�E�N���X�̖��O
		nullptr									//�������A�C�R�����ݒ肳�ꂽ�ꍇ�̏����L�q
	};

	//�E�B���h�E�N���X�̓o�^(�I���W�i���̃E�B���h�E�̖��O�̓o�^)
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

	//�E�B���h�E���쐬
	*phWnd = CreateWindowEx(0,                    //�g���E�B���h�E�X�^�C��
		CLASS_NAME,								//�E�B���h�E�N���X�̖��O
		WINDOW_NAME,							//�E�B���h�E�̖��O
		WS_OVERLAPPEDWINDOW ^ (WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX),					//�E�B���h�E�X�^�C��
		wx,							            //�E�B���h�E�̍���X���W
		wy,							            //�E�B���h�E�̍���Y���W
		ww,							            //�E�B���h�E�̕�
		wh,							            //�E�B���h�E�̍���
		nullptr,									//�e�E�B���h�E�̃n���h��
		nullptr,									//���j���[�n���h���܂��͎q�E�B���h�EID
		hInstance,								//�C���X�^���X�n���h��
		nullptr);									//�E�B���h�E�쐬�f�[�^


	//�E�B���h�E�̕\��
	ShowWindow(*phWnd, nCmdShow);					//�w�肳�ꂽ�E�C���h�E�̕\��
	UpdateWindow(*phWnd);							//�E�B���h�E�̏�Ԓ����ɕԂ�
}

void MainLoop(MSG *msg, const HWND& hWnd)
{
	//����\�̐ݒ�
	timeBeginPeriod(1);

	//�e�J�E���^�[�̏�����
	DWORD dwFrameCount = 0;
	DWORD dwCurrentTime = 0;
	DWORD dwExecLastTime = timeGetTime();                  //���݂̎������~���b�P��
	DWORD dwFPSLastTime = dwExecLastTime;

	//���b�Z�[�W���[�v
	for (;;)
	{
		if (PeekMessage(msg, nullptr, 0, 0, PM_REMOVE) != 0)
		{
			//Windows�̏���
			if (msg->message == WM_QUIT) break;
			else
			{
				TranslateMessage(msg);               //�Ăяo�����X���b�h�̃��b�Z�[�W�L���[��
				DispatchMessage(msg);                //���b�Z�[�W�̖|��Ƒ��o
			}
		}

		//�Q�[���̏���
		else
		{
			dwCurrentTime = timeGetTime();

			if (dwCurrentTime - dwFPSLastTime >= 500)
			{
				g_nCountFPS = ((dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime));
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}

			//1�t���[���̎��Ԃ��o��������Q�[�����[�v
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
				dwExecLastTime = dwCurrentTime;

				//DirectX�̏���
				CMousePosition::GetMousePosFromWindow(hWnd);                       //�}�E�X�̍��W�ۑ�
				CTitle::SetWnd(hWnd);
				ImGui_ImplDX9_NewFrame();				//imgui�J�n
				g_SceneTra->Update();
				g_SceneTra->Draw();
				dwFrameCount++;
			}
		}
	}

	//����\��߂�
	timeEndPeriod(1);
}

//-----------------------------------------------------------------------------------------
//�E�B���h�E�v���V�[�W��
//(CALLBACK:Win32API�֐����Ăяo�����̋K��)
//hWnd   :�E�B���h�E�̃n���h��
//uMsg   :���b�Z�[�W�̎��ʎq
//wParam :���b�Z�[�W�̍ŏ��̃p�����[�^
//LParam :���b�Z�[�W��2�Ԗڂ̃p�����[�^
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
		PostQuitMessage(0);							//"WM_QUIT"���b�Z�[�W��Ԃ�
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

	return DefWindowProc(hWnd, uMsg, wParam, lParam);  //����̏������(�E�B���h�E�̊�{�̓�����񋟂���(����͂Ȃ��Ɗ댯�I))
}