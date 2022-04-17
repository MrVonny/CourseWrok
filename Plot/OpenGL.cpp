#include "OpenGL.h"

using namespace OpenGLForm;


void renderText(char *txt, float x, float y, float z)
{
	char text[32];
	sprintf_s(text, txt);
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos3f(x, y, z);
	for (int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}


COpenGL::COpenGL(System::Windows::Forms::GroupBox ^ parentForm, GLsizei iWidth, GLsizei iHeight)
{
	CreateParams^ cp = gcnew CreateParams;

	cp->X = 0;
	cp->Y = 0;
	cp->Height = iHeight;
	cp->Width = iWidth;

	cp->Parent = parentForm->Handle;

	cp->Style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	this->CreateHandle(cp);

	m_hDC = GetDC((HWND)this->Handle.ToPointer());

	if (m_hDC)
	{
		MySetPixelFormat(m_hDC);
		ReSizeGLScene(iWidth, iHeight);
		InitGL();
		InitUserSettings();
	}
}

COpenGL::~COpenGL()
{
	this->DestroyHandle();
}

void COpenGL::SwapOpenGLBuffers()
{
	SwapBuffers(m_hDC);
}

bool COpenGL::InitGL(){								
	glShadeModel(GL_SMOOTH);							
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				
	glClearDepth(1.0f);									
	glEnable(GL_DEPTH_TEST);							
	glDepthFunc(GL_LEQUAL);								
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	

	return true;										
}


GLvoid COpenGL::ReSizeGLScene(GLsizei width, GLsizei height)		
{
	if (height == 0)										
	{
		height = 1;										
	}

	glViewport(0, 0, width, height);						

	glMatrixMode(GL_PROJECTION);					
	glLoadIdentity();								
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);							
	glLoadIdentity();								
}


GLint COpenGL::MySetPixelFormat(HDC hdc)
{
	static	PIXELFORMATDESCRIPTOR pfd =				
	{
		sizeof(PIXELFORMATDESCRIPTOR),				
		1,											
		PFD_DRAW_TO_WINDOW |						
		PFD_SUPPORT_OPENGL |						
		PFD_DOUBLEBUFFER,							
		PFD_TYPE_RGBA,								
		16,										
		0, 0, 0, 0, 0, 0,							
		0,											
		0,											
		0,											
		0, 0, 0, 0,									
		16,											  
		0,											
		0,											
		PFD_MAIN_PLANE,								
		0,											
		0, 0, 0										
	};

	GLint  iPixelFormat;

	
	if ((iPixelFormat = ChoosePixelFormat(hdc, &pfd)) == 0)
	{
		MessageBox::Show("ChoosePixelFormat Failed");
		return 0;
	}

	
	if (SetPixelFormat(hdc, iPixelFormat, &pfd) == FALSE)
	{
		MessageBox::Show("SetPixelFormat Failed");
		return 0;
	}

	if ((m_hglrc = wglCreateContext(m_hDC)) == NULL)
	{
		MessageBox::Show("wglCreateContext Failed");
		return 0;
	}

	if ((wglMakeCurrent(m_hDC, m_hglrc)) == NULL)
	{
		MessageBox::Show("wglMakeCurrent Failed");
		return 0;
	}


	return 1;
}

bool COpenGL::InitUserSettings(){
	minView.x = -10;
	minView.y = -10;
	minView.z = -10;

	maxView.x = 10;
	maxView.y = 10;
	maxView.z = 10;

	return true;
}


void COpenGL::MouseEvents(System::Windows::Forms::Label^ label, System::Windows::Forms::GroupBox^ Container){
	POINT mouse;	
	GetCursorPos(&mouse);	

	ScreenToClient((HWND)this->Handle.ToPointer(), &mouse);	

	
	if (mouse.x > 0 && mouse.y > 0 && mouse.x < Container->Width && mouse.y < Container->Height){
		
		
		if (GetAsyncKeyState(VK_LBUTTON) && !GetAsyncKeyState(VK_CONTROL)){

			if (mouseCoord.x == 0 && mouseCoord.y == 0){
				mouseCoord.x = (float)mouse.x;
				mouseCoord.y = (float)mouse.y;
			}

			mouseDif.x = resultDif.x + mouse.x - mouseCoord.x;
			mouseDif.y = resultDif.y + mouse.y - mouseCoord.y;
		}

		else if (GetAsyncKeyState(VK_RBUTTON)){
			if (mouseCoord.x == 0 && mouseCoord.y == 0){
				mouseCoord.x = (float)mouse.x;
				mouseCoord.y = (float)mouse.y;
			}

			translateDif.x = translateResult.x + mouse.x - mouseCoord.x;
			translateDif.y = translateResult.y + mouse.y- mouseCoord.y;
		}

		else if (GetAsyncKeyState(VK_LBUTTON) && GetAsyncKeyState(VK_CONTROL)){
			
			if (mouseCoord.z == 0){
				mouseCoord.z = (float)mouse.y;
			}

			mouseDif.z = resultDif.z + mouse.y - mouseCoord.z;
		}

		
		else{
			mouseCoord.x = 0;
			mouseCoord.y = 0;
			mouseCoord.z = 0;

			resultDif.x = mouseDif.x;
			resultDif.y = mouseDif.y;
			resultDif.z = mouseDif.z;

			translateResult.x = translateDif.x;
			translateResult.y = translateDif.y;
		
		}
	}

}

void COpenGL::Render(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();									
	glTranslatef(0.f, 0.0f, -50.0f);						

	
	glTranslatef(translateDif.x*0.05, -translateDif.y*0.05, -mouseDif.z*0.5f);

	
	glRotatef(mouseDif.y, 1.0f, 0, 0);
	glRotatef(mouseDif.x, 0, 1.0f, 0);

	
	renderText("-x", minView.x - fontDistance, 0, 0);
	renderText("x", maxView.x + fontDistance, 0, 0);

	renderText("-y", 0, minView.y - fontDistance, 0);
	renderText("y", 0, maxView.y + fontDistance, 0);

	renderText("-z", 0, 0, minView.z - fontDistance);
	renderText("z", 0, 0, maxView.z + fontDistance);


	
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
		
		for (float x = minView.x; x <= maxView.x; x++){
			glVertex3f(x, minView.y, 0.0f);
			glVertex3f(x, maxView.y, 0.0f);

			glVertex3f(x, 0, minView.z);
			glVertex3f(x, 0, maxView.z);
		}
		
		for (float y = minView.y; y <= maxView.y; y++){
			glVertex3f(minView.x, y, 0.0f);
			glVertex3f(maxView.x, y, 0.0f);

			glVertex3f(0, y, minView.z);
			glVertex3f(0, y, maxView.z);
		}
		
		for (float z = minView.z; z <= maxView.z; z++){
			glVertex3f(0, minView.y, z);
			glVertex3f(0, maxView.y, z);

			glVertex3f(minView.x, 0, z);
			glVertex3f(maxView.x, 0, z);
		}
		
	glEnd();

	if (bIsDoneParsing){
			glPointSize(3.5f);

		glBegin(GL_POINTS);
		
		
		for (int k = 0; k < curGraphNo; k++){
			glColor3f(graphList[k].r, graphList[k].g, graphList[k].b);
			for (int i = 0; i <= graphList[k].plotRange; i++){
				if (graphList[k].parsedCoordinates[i].x >= minView.x && graphList[k].parsedCoordinates[i].x <= maxView.x)
					if (graphList[k].parsedCoordinates[i].y >= minView.y && graphList[k].parsedCoordinates[i].y <= maxView.y)
						if (graphList[k].parsedCoordinates[i].z >= minView.z && graphList[k].parsedCoordinates[i].z <= maxView.z)
							glVertex3f(graphList[k].parsedCoordinates[i].x, graphList[k].parsedCoordinates[i].y, graphList[k].parsedCoordinates[i].z);
			}
		}

		
		for (int k = 0; k < curGraphNo; k++){
			glColor3f(graphList[k].r, graphList[k].g, graphList[k].b);
			if (graphList[k].isComplex){
				for (int i = 0; i <= graphList[k].plotRange; i++){
					if (graphList[k].complexCoordinates[i].x >= minView.x && graphList[k].complexCoordinates[i].x <= maxView.x)
						if (graphList[k].complexCoordinates[i].y >= minView.y && graphList[k].complexCoordinates[i].y <= maxView.y)
							if (graphList[k].complexCoordinates[i].z >= minView.z && graphList[k].complexCoordinates[i].z <= maxView.z)
								glVertex3f(graphList[k].complexCoordinates[i].x, graphList[k].complexCoordinates[i].y, graphList[k].complexCoordinates[i].z);
				}
			}
		}
		
		
		for (int k = 0; k < curGraphNo; k++){
			glColor3f(graphList[k].r, graphList[k].g, graphList[k].b);
			if (graphList[k].isComplexCubic){
				for (int i = 0; i <= graphList[k].plotRange; i++){
					if (graphList[k].complexCoordinatesCubic[i].x >= minView.x && graphList[k].complexCoordinatesCubic[i].x <= maxView.x)
						if (graphList[k].complexCoordinatesCubic[i].y >= minView.y && graphList[k].complexCoordinatesCubic[i].y <= maxView.y)
							if (graphList[k].complexCoordinatesCubic[i].z >= minView.z && graphList[k].complexCoordinatesCubic[i].z <= maxView.z)
								glVertex3f(graphList[k].complexCoordinatesCubic[i].x, graphList[k].complexCoordinatesCubic[i].y, graphList[k].complexCoordinatesCubic[i].z);
				}
			}
		}


		glEnd();
	}

}

Void COpenGL::cameraViewLabel(System::Windows::Forms::ToolStripStatusLabel^ label){
	label->Text = "Camera - x:" + mouseDif.x.ToString() + " y: " + mouseDif.y.ToString() + " z: " + mouseDif.z.ToString();
}