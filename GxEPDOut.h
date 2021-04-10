/********************

Based on U8GLibOut.h from Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
Original from: https://github.com/christophepersoz

***/
#ifndef RSITE_ARDUINOP_MENU_GxEPD2_213_B72
#define RSITE_ARDUINOP_MENU_GxEPD2_213_B72
#include <GxEPD2_BW.h>
#include "menuDefs.h"


namespace Menu {

#define RGB565(r,g,b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))

	class GxEPDOut :public gfxOut {
	public:
		int8_t offsetX = 0;
		int8_t offsetY = 0;
		int8_t fontOffsetY = 0;
		Screen& gfx;
		const colorDef<uint16_t>(&colors)[nColors];
		
		
		GxEPDOut(
			Screen    &gfx                                   ,
			const       colorDef<uint16_t>(&palette)[nColors],
			idx_t      *menuIndices                          ,
			panelsList &panels                               ,
			idx_t       resX        = 6                      ,
			idx_t       resY        = 9                      ,
			idx_t       offsetX     = 0                      ,
			idx_t       offsetY     = 0                      ,
			idx_t       fontOffsetY = 0                      ,
			int         fontMarginX = 1                      ,
			int         fontMarginY = 1
		) :gfxOut(resX, resY, menuIndices, panels, (styles)(menuOut::redraw | menuOut::rasterDraw)), gfx(gfx), colors(palette) {
			this->offsetX     = offsetX    ;
			this->offsetY     = offsetY    ;
			this->fontMarginX = fontMarginY;
			this->fontMarginY = fontMarginY;
			this->fontOffsetY = fontOffsetY;
		}

		size_t write(uint8_t ch) override
		{
			return gfx.write(ch);
		}

		inline uint16_t getColor(colorDefs color = bgColor, bool selected = false, status stat = enabledStatus, bool edit = false) const
		{
			return memWord(&(stat == enabledStatus ? colors[color].enabled[selected + edit] : colors[color].disabled[selected]));
		}

		void setColor(colorDefs c, bool selected = false, status s = enabledStatus, bool e = false) override {
			if (c == titleColor)
			{
				gfx.setTextColor(getColor(titleColor, selected, s, e), getColor(titleColor, false, s, e));
			}
			else
			{
				gfx.setTextColor(getColor(c, selected, s, e), getColor(bgColor, selected, s, e));
			}
		}

		void clearLine(idx_t ln, idx_t panelNr = 0, colorDefs color = bgColor, bool selected = false, status stat = enabledStatus, bool edit = false) override
		{
			const panel p = panels[panelNr];
			gfx.fillRect(p.x * resX + offsetX, (p.y + ln) * resY + offsetY , p.maxX() * resX, resY, getColor(color, selected, stat, edit));
			//setCursor(0,ln);
		}

		//void clear() override {
		//	panels.reset();
		//	gfx.fillScreen(getColor(bgColor, false, enabledStatus, false));
		//	setCursor(0, 0);
		//	setColor(fgColor);
		//}

		void clear() override {
			clear(0);			
			panels.reset();
		}

		void clear(idx_t panelNr) override
		{
			const panel p = panels[panelNr];
			gfx.fillRect(p.x * resX + offsetX, p.y * resY + offsetY, p.w * resX, p.h * resY, getColor(bgColor, false, enabledStatus, false));
			panels.nodes[panelNr] = nullptr;
		}

		void box(idx_t panelNr, idx_t x, idx_t y, idx_t w = 1, idx_t h = 1, colorDefs c = bgColor, bool selected = false, status stat = enabledStatus, bool edit = false) override
		{
			const panel p = panels[panelNr];
			gfx.drawRect((p.x + x) * resX, (p.y + y) * resY, w * resX, h * resY, getColor(c, selected, stat, edit));
		}

		void rect(idx_t panelNr, idx_t x, idx_t y, idx_t w = 1, idx_t h = 1, colorDefs c = bgColor, bool selected = false, status stat = enabledStatus, bool edit = false) override
		{
			const panel p = panels[panelNr];
			gfx.fillRect((p.x + x) * resX, (p.y + y) * resY, w * resX, h * resY, getColor(c, selected, stat, edit));
		}

		void setCursor(idx_t x, idx_t y, idx_t panelNr = 0) override
		{
			const panel p = panels[panelNr];
			gfx.setCursor((p.x + x) * resX + fontMarginX + offsetX, 
				          (p.y + y) * resY + fontMarginY + offsetY+ fontOffsetY);
		}


		void drawCursor(idx_t ln, bool selected, status stat, bool edit = false, idx_t panelNr = 0) override
		{
			const panel p = panels[panelNr];
			const int cursorOffsetY = 0;
			// gfxOut::drawCursor(ln,selected,stat);
			gfx.drawRect(p.x * resX + offsetX, (p.y + ln) * resY + offsetY + cursorOffsetY, maxX() * resX, resY, getColor(cursorColor, selected, enabledStatus, false));
		}

	};
}
#endif 
