#pragma once

#include <Dialogs/DialogInterface.hpp>

class [[nodiscard]] JoinGameDialog final : public DialogInterface
{
public:
    JoinGameDialog(
        mem::Rc<Gui> gui,
        const std::string& defaultIp,
        std::function<void(const std::string& ip)> tryJoinGame);

protected:
    std::expected<ReturnFlag, ErrorMessage>
    validateBeforeConfirmation() const override;

    void buildLayoutImpl(tgui::Panel::Ptr target) override;

private:
    std::string joinIp;
    std::function<void(const std::string& ip)> tryJoinGame;
};
