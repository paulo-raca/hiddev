#include <hiddev/core.h>

namespace hiddev {
	namespace device {
		enum class MouseButton : uint8_t {
			None   = 0,
			Left   = 1,
			Right  = 2,
			Middle = 4,
			Prev   = 8,
			Next   = 16
		};


		struct MouseReport {
			MouseButton buttons;
			int8_t x;
			int8_t y;
			int8_t wheel;
		};


		class Mouse: public hiddev::Device {
		private:
			MouseButton buttons;

		protected:
			// Hiddev API
			void getDescriptor(const uint8_t* &descriptorBuffer, uint16_t &descriptorSize) override;
			bool getReport(hiddev::ReportType reportType, uint8_t reportNum, uint8_t* reportBuffer, uint16_t &reportSize) override;
			bool isNumberedReport(hiddev::ReportType reportType) override;
			uint16_t getReportSize(hiddev::ReportType reportType, uint8_t reportNum) override;

		public:
			// Mouse API
			void move(int8_t x, int8_t y, int8_t wheel = 0);
			void click(MouseButton button = MouseButton::Left);
			void press(MouseButton button = MouseButton::Left);
			void release(MouseButton button = MouseButton::Left);
			void releaseAll();
			bool isPressed(MouseButton button = MouseButton::Left);

		};
	}
}
