#include "WindowPainter.hpp"

void WindowPainter::LoadImageFromFile(std::wstring path) {
	this->fieldImage = Image::FromFile(path.c_str());
}

void WindowPainter::DrawField() {
	INT height = this->windowRect.bottom - this->windowRect.top;
	INT width = (this->fieldImage->GetWidth() / (this->fieldImage->GetHeight() * 1.0)) * height;

	this->currentGraphics->DrawImage(this->fieldImage, 0, 0, width, height);
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