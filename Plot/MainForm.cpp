#include "MainForm.h"

using namespace Plot;

[STAThreadAttribute]

int main(){
	graphList = new GraphList[MAX_NO_GRAPHS];

	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	Application::Run(gcnew MainForm());
	return 0;
}

Void MainForm::timer1_Tick(System::Object^  sender, System::EventArgs^  e){
	UNREFERENCED_PARAMETER(sender);
	UNREFERENCED_PARAMETER(e);
	OpenGL->Render();
	OpenGL->SwapOpenGLBuffers();
	OpenGL->MouseEvents(nullptr, OpenGL_Container);
	OpenGL->cameraViewLabel(cameraStatusStrip);
}

Void MainForm::debugButton_Click(System::Object^  sender, System::EventArgs^  e){
}

Void MainForm::defaultView_Click(System::Object^  sender, System::EventArgs^  e){
	minView.x = -10;
	minView.y = -10;
	minView.z = -10;

	maxView.x = 10;
	maxView.y = 10;
	maxView.z = 10;

	resultDif.x = 0;
	resultDif.y = 0;
	resultDif.z = 0;

	mouseDif.x = 0;
	mouseDif.y = 0;
	mouseDif.z = 0;

	translateDif.x = 0;
	translateDif.y = 0;

	translateResult.x = 0;
	translateResult.y = 0;
}

Void MainForm::equationTextbox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e){
	if (e->KeyChar == (char)13){
		if (equationTextbox->Text != ""){
			plotButton->PerformClick();
		}
	}
}

Void MainForm::plotButton_Click(System::Object^  sender, System::EventArgs^  e){

	graphList[curGraphNo].isComplex = false;
	graphList[curGraphNo].isComplexCubic = false;
	
	graphList[curGraphNo].r = randomFloat(0, 1);
	graphList[curGraphNo].g = randomFloat(0, 1);
	graphList[curGraphNo].b = randomFloat(0, 1);

	graphList[curGraphNo].plotRange = abs(minView.x) + abs(maxView.x) + 2;
	graphList[curGraphNo].plotRange *= graphDensity;
	if (!graphList[curGraphNo].isComplex){
		graphList[curGraphNo].plotRange *= graphList[curGraphNo].plotRange;	
	}

	graphList[curGraphNo].parsedCoordinates = new Coordinates[(int)graphList[curGraphNo].plotRange];	

	// Initializes complex graphs
	if (graphList[curGraphNo].isComplex){
		graphList[curGraphNo].complexCoordinates = new Coordinates[(int)graphList[curGraphNo].plotRange];
	}

	if (graphList[curGraphNo].isComplexCubic){
		graphList[curGraphNo].complexCoordinatesCubic = new Coordinates[(int)graphList[curGraphNo].plotRange];
	}

	bIsDoneParsing = false;

	if (!graphList[curGraphNo].isComplex){
		size_t strPlace = 0;

		std::string str, strSubject, strEquation;	
		MarshalString(equationTextbox->Text, str);	

		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		removeWhitespace(str);
		strPlace = str.find("=");

		if (strPlace == -1){
			MessageBox::Show("Нет символа '='");
			return;
		}


		strSubject = str.substr(0, strPlace);
		strEquation = str.substr(strPlace + 1);

		parseEquation<float>(strSubject, strEquation);	// Parses the string 

		graphListView->Items->Add(gcnew String(str.c_str()));
		graphListView->Items[curGraphNo]->ForeColor = Color::FromArgb(1, (int)(graphList[curGraphNo].r * 255), (int)(graphList[curGraphNo].g * 255), (int)(graphList[curGraphNo].b * 255));
	}


	bIsDoneParsing = true;

	curGraphNo++;

}

Void MainForm::clearAllButton_Click(System::Object^  sender, System::EventArgs^  e){
	curGraphNo = 0;
	graphListView->Items->Clear();
}

Void MainForm::removeSelectedButton_Click(System::Object^  sender, System::EventArgs^  e){
	int index = -1;

	for (int i = 0; i < graphListView->Items->Count; i++){
		if (graphListView->Items[i]->Selected){
			index = i;
			break;
		}
	}

	if (index != -1){
		graphListView->Items[index]->Remove();

		for (int i = index; i < curGraphNo; i++){
			graphList[i] = graphList[i + 1];
		}

		curGraphNo--;
	}
}

Void MainForm::toggleColorButton_Click(System::Object^  sender, System::EventArgs^  e){
	int index = -1;

	for (int i = 0; i < graphListView->Items->Count; i++){
		if (graphListView->Items[i]->Selected){
			index = i;
			break;
		}
	}

	if (index != -1){
		graphList[index].r = randomFloat(0, 1);
		graphList[index].g = randomFloat(0, 1);
		graphList[index].b = randomFloat(0, 1);

		graphListView->Items[index]->ForeColor = Color::FromArgb(1, (int)(graphList[index].r * 255), (int)(graphList[index].g * 255), (int)(graphList[index].b * 255));
	}
}

Void MainForm::graphDensityBar_Scroll(System::Object^  sender, System::EventArgs^  e){
}

Void MainForm::decreaseView_Click(System::Object^  sender, System::EventArgs^  e){
	if (minView.x + 5 < maxView.x - 5){
		minView.x += 5;
		maxView.x -= 5;
	}
	if (minView.y + 5 < maxView.y - 5){
		minView.y += 5;
		maxView.y -= 5;
	}

	if (minView.z + 5 < maxView.z - 5){
		maxView.z -= 5;
		minView.z += 5;
	}

	if(mouseDif.z - 10 > 20)
		mouseDif.z -= 15;
}

Void MainForm::increaseView_Click(System::Object^  sender, System::EventArgs^  e){
	minView.x -= 5;
	minView.y -= 5;
	minView.z -= 5;

	maxView.x += 5;
	maxView.y += 5;
	maxView.z += 5;

	mouseDif.z += 15;
}

Void MainForm::isComplexCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
	equationTextbox->Visible = true;
}

Void MainForm::copyToClipboardToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e){
	int width = OpenGL_Container->Width;
	int height = OpenGL_Container->Height;

	HDC hdcParent = OpenGL->getHDC();
	HDC hdc = CreateCompatibleDC(hdcParent);
	HBITMAP hBmp = CreateCompatibleBitmap(hdcParent, width, height);
	SelectObject(hdc, hBmp);


	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBmp);
	CloseClipboard();
}