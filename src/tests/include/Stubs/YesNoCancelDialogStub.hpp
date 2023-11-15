#include "include/Dialogs/YesNoCancelDialog.hpp"

/**
 * Usage:
 * auto yesNoDialogMock = GC<YesNoCancelDialogStub>();
 * auto yesNoDialogSpy = Mock<YesNoCancelDialogInterface>(*yesNoDialogMock);
 *
 * Use mock for dependency injection and set expectations on the spy object.
 */
class YesNoCancelDialogStub : public YesNoCancelDialogInterface
{
public:
	virtual void open(
		const std::string,
		const std::string&,
		std::function<void(UserChoice)> completedCallback)
	{}
};
