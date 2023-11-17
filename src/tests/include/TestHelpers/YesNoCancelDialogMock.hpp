#include <include/Interfaces/DialogInterfaces.hpp>

class YesNoCancelDialogStub : public YesNoCancelDialogInterface
{
public:
	virtual void open(const std::string, const std::string&, std::function<void(UserChoice)>)
	{
		/*if (userCancelled)
			completedCallback(UserChoice::Cancelled);
		else if (userConfirmed)
			completedCallback(UserChoice::Confirmed);
		else
			completedCallback(UserChoice::Denied);*/
	}
};
