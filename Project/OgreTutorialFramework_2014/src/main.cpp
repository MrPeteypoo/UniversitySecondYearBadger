#include <iostream>
#include <Framework/OgreApplication.h>

int main()
{
    // Enable memory leak detection.
    #ifndef NDEBUG

    int flag = _CrtSetDbgFlag (_CRTDBG_REPORT_FLAG);

    flag |= _CRTDBG_LEAK_CHECK_DF;

    _CrtSetDbgFlag (flag);

    #endif

	//show - hide console window
	HWND hwnd = GetConsoleWindow();
	ShowWindow(hwnd, 1);

	shared_ptr<OgreApplication>  application = make_shared<OgreApplication>();

	// Step 1: Create and initialze the ogre application
	if (application == nullptr || !application->Initialize())
	{
		std::cout << __FILE__ << " (" << __LINE__ << ") - " << "Failed to initialise the application" << std::endl;
		system("PAUSE");
		return 0;
	}

	application->RunOgreApplication();
	
	return 0;
}


