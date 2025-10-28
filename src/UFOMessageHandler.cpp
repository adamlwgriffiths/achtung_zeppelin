#include "UFOMain.h"

/**
* This file defines all of our WIN32 Message handlers
*/


/**
* Message handler for about box.
*/
LRESULT CALLBACK AboutProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			return true;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK  ||  LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return true;
			}
		break;
	}
	return false;
}


/**
* Message handler for about box.
*/
LRESULT CALLBACK HelpProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			return true;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK  ||  LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return true;
			}
		break;
	}
	return false;
}





/**
* Message handler for about box.
*/
LRESULT CALLBACK SettingsProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char buff[20];
	int length;

	switch (uMsg)
	{
		case WM_INITDIALOG:
//			if ( CUFOMain::m_bFlatWorld )
//				CheckRadioButton( hDlg, IDC_RADIO_TERRAIN_FLAT, IDC_RADIO_TERRAIN_SPHERE, IDC_RADIO_TERRAIN_FLAT );
//			else
//				CheckRadioButton( hDlg, IDC_RADIO_TERRAIN_FLAT, IDC_RADIO_TERRAIN_SPHERE, IDC_RADIO_TERRAIN_SPHERE );

			CheckDlgButton( hDlg, IDC_DEMO_MODE, CUFOMain::m_bDemoMode );

			// use sprintf to convert to string since _itot is not crossplatform
			sprintf( buff, "%i", CUFOMain::m_iMaxNumShips );
			SetDlgItemText( hDlg, IDC_GAME_NUMSHIPS, buff );

			sprintf( buff, "%i", CUFOMain::m_iNumCities );
			SetDlgItemText( hDlg, IDC_GAME_NUMCITIES, buff );

			sprintf( buff, "%f", CUFOMain::m_fMeshHeightScale );
			SetDlgItemText( hDlg, IDC_HEIGHT_SCALE, buff );

			sprintf( buff, "%i", CUFOMain::m_iTexSize );
			SetDlgItemText( hDlg, IDC_TEXTURE_SIZE, buff );

			sprintf( buff, "%i", CUFOMain::m_iAmplitude );
			SetDlgItemText( hDlg, IDC_FRACTAL_AMPLITUDE, buff );

			sprintf( buff, "%f", CUFOMain::m_fRoughness );
			SetDlgItemText( hDlg, IDC_FRACTAL_ROUGHNESS, buff );

			sprintf( buff, "%i", CUFOMain::m_iMeshWidth );
			SetDlgItemText( hDlg, IDC_FLAT_WIDTH, buff );

			sprintf( buff, "%i", CUFOMain::m_iMeshLength );
			SetDlgItemText( hDlg, IDC_FLAT_LENGTH, buff );

			sprintf( buff, "%i", CUFOMain::m_iMeshSpacing );
			SetDlgItemText( hDlg, IDC_FLAT_SPACING, buff );

//			sprintf( buff, "%f", CUFOMain::m_fRadius );
//			SetDlgItemText( hDlg, IDC_SPHERICAL_RADIUS, buff );

//			sprintf( buff, "%i", CUFOMain::m_iNumSlices );
//			SetDlgItemText( hDlg, IDC_SPHERICAL_SLICES, buff );

//			sprintf( buff, "%i", CUFOMain::m_iNumStacks );
//			SetDlgItemText( hDlg, IDC_SPHERICAL_STACKS, buff );

			return true;

		case WM_COMMAND:
			if ( LOWORD(wParam) == IDOK )
			{
//				if (  IsDlgButtonChecked( hDlg, IDC_RADIO_TERRAIN_FLAT )  )
//					CUFOMain::m_bFlatWorld = true;
//				else
//					CUFOMain::m_bFlatWorld = false;

				if (  IsDlgButtonChecked( hDlg, IDC_DEMO_MODE )  )
					CUFOMain::m_bDemoMode = true;
				else
					CUFOMain::m_bDemoMode = false;


				length = GetDlgItemText( hDlg, IDC_GAME_NUMSHIPS, buff, 8 );
				if ( length > 0 )
				{
					if ( atoi( buff ) > 0  &&  atoi( buff ) < SHIPS_MAX_NUM )
						CUFOMain::m_iMaxNumShips = atoi( buff );
				}

				length = GetDlgItemText( hDlg, IDC_GAME_NUMCITIES, buff, 8 );
				if ( length > 0 )
				{
					if ( atoi( buff ) > 0 )
						CUFOMain::m_iNumCities = atoi( buff );
				}



				length = GetDlgItemText( hDlg, IDC_HEIGHT_SCALE, buff, 8 );
				if ( length > 0 )
				{
					if (  (float)atof( buff ) > 0.0f  )
						CUFOMain::m_fMeshHeightScale = (float)atof( buff );
				}

				length = GetDlgItemText( hDlg, IDC_TEXTURE_SIZE, buff, 8 );
				if ( length > 0 )
				{
					if (  atoi( buff ) > 0  )
						CUFOMain::m_iTexSize = atoi( buff );
				}

				length = GetDlgItemText( hDlg, IDC_FRACTAL_AMPLITUDE, buff, 8 );
				if ( length > 0 )
				{
					if (  atoi( buff )  > 0 )
						CUFOMain::m_iAmplitude = atoi( buff );
				}

				length = GetDlgItemText( hDlg, IDC_FRACTAL_ROUGHNESS, buff, 8 );
				if ( length > 0 )
				{
					if (  (float)atof( buff ) > 0.0f  )
						CUFOMain::m_fRoughness = (float)atof( buff );
				}

				length = GetDlgItemText( hDlg, IDC_FLAT_WIDTH, buff, 8 );
				if ( length > 0 )
				{
					if (  atoi( buff ) > 1  )
						CUFOMain::m_iMeshWidth = atoi( buff );
				}

				length = GetDlgItemText( hDlg, IDC_FLAT_LENGTH, buff, 8 );
				if ( length > 0 )
				{
					if (  atoi( buff ) > 1  )
						CUFOMain::m_iMeshLength = atoi( buff );
				}

				length = GetDlgItemText( hDlg, IDC_FLAT_SPACING, buff, 8 );
				if ( length > 0 )
				{
					if (  atoi( buff ) > 0  )
						CUFOMain::m_iMeshSpacing = atoi( buff );
				}

//				length = GetDlgItemText( hDlg, IDC_SPHERICAL_RADIUS, buff, 8 );
//				if ( length > 0 )
//				{
//					if (  (float)atof( buff ) > 0.0f  )
//						CUFOMain::m_fRadius = (float)atof( buff );
//				}

//				length = GetDlgItemText( hDlg, IDC_SPHERICAL_SLICES, buff, 8 );
//				if ( length > 0 )
//				{
//					if (  atoi( buff ) > 3  )
//						CUFOMain::m_iNumSlices = atoi( buff );
//				}

//				length = GetDlgItemText( hDlg, IDC_SPHERICAL_STACKS, buff, 8 );
//				if ( length > 0 )
//				{
//					if (  atoi( buff ) > 2  )
//						CUFOMain::m_iNumStacks = atoi( buff );
//				}

				// Restart the game
				CUFOMain::Get()->PostMessage(MSGID_RESTART, NULL);

				EndDialog(hDlg, LOWORD(wParam));
				return true;
			}
			else if ( LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return true;
			}
		break;
	}
	return false;
}







/**
* Our event handler that will handle menu events, send key commands
* to our InputController and will send any unknown messages to the D3DApp
*/
LRESULT CUFOMain::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
//	// Trap context menu
//	if (WM_CONTEXTMENU == uMsg)
//		return 0;

	int wmId, wmEvent;

	switch (uMsg)
	{
		case WM_KEYDOWN:							// Handle keyboard input
			switch (wParam)
			{
				case VK_F2:
					// Restart the game
					CUFOMain::Get()->PostMessage(MSGID_RESTART, NULL);
					break;

#ifdef INPUT_WINEVENT
				////////////////////////////////
				// Windows Event Input
				default:
					CGameController::Get()->PostMessage(MSGID_INPUT_KEY_PRESSED, &wParam);
					break;
#endif // #ifdef INPUT_WINEVENT

			}
			break;

		case WM_KEYUP:
#ifdef INPUT_WINEVENT
			CGameController::Get()->PostMessage(MSGID_INPUT_KEY_RELEASED, &wParam);
#endif // #ifdef INPUT_WINEVENT
			break;


////////////////////////////////
// Menu Events
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);

			switch( wmId )
			{
				// create a new game
				case ID_FILE_NEW:
					this->Restart();
					break;

				// show our ABOUT dialog
				case ID_HELP_ABOUT:
					DialogBox(hInst, (LPCTSTR)IDD_HELP_ABOUT, hWnd, (DLGPROC)AboutProc);
					break;

				case ID_HELP_HELP:
					DialogBox(hInst, (LPCTSTR)IDD_HELP_HELP, hWnd, (DLGPROC)HelpProc);
					break;

				// show our SETTINGS dialog
				case ID_SETTINGS_WORLDCREATION:
					DialogBox(hInst, (LPCTSTR)IDD_SETTINGS_WORLDCREATION, hWnd, (DLGPROC)SettingsProc);
					break;

				// Switch camera modes
				case ID_GAME_CHANGEVIEW:
					CInputController::Get()->PostMessage(MSGID_CAMERA_SWITCHVIEW, NULL);
					break;
			}
			break;


	}

	// Pass remaining messages to default handler
	return CD3DApplication::MsgProc(hWnd, uMsg, wParam, lParam);
}