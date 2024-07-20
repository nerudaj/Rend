#pragma once

#include "DialogInterface.hpp"
#include "FormBuilder.hpp"
#include "MetadataDialogBase.hpp"
#include <DGM/dgm.hpp>
#include <LevelMetadata.hpp>
#include <Memory.hpp>

class [[nodiscard]] EditMetadataDialog final
    : protected DialogInterface
    , public MetadataDialogBase
{
public:
    EditMetadataDialog(mem::Rc<Gui> gui);

public:
    void
    open(const LevelMetadata& metadata, std::function<void()> confirmCallback);

protected:
    void buildLayoutImpl(tgui::Panel::Ptr panel) override;
};