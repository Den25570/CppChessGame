#include "WindowPainter.hpp"

void WindowPainter::LoadImageFromFile(std::wstring path) {
	//To DO: разные поля с разными характеристиками
	this->fieldImage = Image::FromFile(path.c_str());

	this->boardInfo.width = 800;
	this->boardInfo.height = 800;
	this->boardInfo.topOffset = 18;
	this->boardInfo.leftOffset = 19;
	this->boardInfo.bottomOffset = 21;
	this->boardInfo.rightOffest = 20;
	this->boardInfo.cellWidth = 95;
	this->boardInfo.cellHeight = 95;

}

void WindowPainter::DrawField() {
	INT height = this->windowRect.bottom - this->windowRect.top;
	INT width = (this->fieldImage->GetWidth() / (this->fieldImage->GetHeight() * 1.0)) * height;

	this->boardSizeMult = (1.0f * height) / (this->boardInfo.height);

	this->currentGraphics->DrawImage(this->fieldImage, 0, 0, width, height);
}

void WindowPainter::DrawFigures() {
	Pen bluePen(Color(255, 0, 0, 255));
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			RectF ellipseRect(
				(this->boardInfo.topOffset + i * this->boardInfo.cellWidth + this->boardInfo.cellWidth / 2.0f) * this->boardSizeMult,
				(this->boardInfo.leftOffset + j * this->boardInfo.cellHeight + this->boardInfo.cellHeight / 2.0f)* this->boardSizeMult,
				40.0f, 40.0f);
			this->currentGraphics->DrawEllipse(&bluePen, ellipseRect);
		}
	}
	
}

void WindowPainter::SetHDC(HDC hdc) {
	if (this->currentGraphics != nullptr)
		this->currentGraphics->~Graphics();
	this->currentHDC = hdc;
	this->currentGraphics = Graphics::FromHDC(hdc);
}

void WindowPainter::SetWindow(HWND hwnd) {
	GetClientRect(hwnd, &this->windowRect);
}

void WindowPainter::FreeResources() {
	if (this->currentGraphics != nullptr)
		this->currentGraphics->~Graphics();
	if (this->fieldImage != nullptr)
		this->fieldImage->~Image();
}