#include <inttypes.h>

namespace hiddev {

	enum class ReportType: uint8_t {
		Invalid = 0,
		Input = 1,
		Output = 2,
		Feature = 3
	};

	enum class Protocol: uint8_t {
		Boot = 0,
		Report = 1
	};

	class HidDevice;
	class HidDriver;



	class HidDriver {
		friend HidDevice;
	protected:
		HidDevice &device;
		virtual bool sendInputReport(uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize) = 0;

	public:
		HidDriver(HidDevice &device);
		~HidDriver();
	};



	class HidDevice {
		friend HidDriver;
	protected:
		HidDriver* driver;
	public:
		// Returns the HID descriptor.
		// The buffers must be managed by the instance and will NOT be deallocated by the caller.
		virtual void getDescriptor(const uint8_t* &descriptorBuffer, uint16_t &descriptorSize) = 0;

		// Notifies that the device has been attached / dettached to/from the host
		virtual void start();
		virtual void stop();

		// Asynchronous IO
		bool sendInputReport(uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize);
		virtual bool receivedOutputReport(uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize);

		// Synchronous IO
		// on getInputReport and getFeatureReport, the buffers must be managed by the instance
		virtual bool getReport(ReportType reportType, uint8_t reportNum, const uint8_t* &reportBuffer, uint16_t &reportSize);
		virtual bool setReport(ReportType reportType, uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize);

		virtual bool getIdle(uint16_t &period_ms);
		virtual bool setIdle(uint16_t period_ms);

		virtual bool getProtocol(Protocol &protocol);
		virtual bool setProtocol(Protocol protocol);

		virtual bool isNumberedReport(ReportType reportType);
	};

}
