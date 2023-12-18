#pragma once

#include "DialogBase.hpp"
#include "MetadataDialogBase.hpp"
#include <DGM/dgm.hpp>
#include <LevelMetadata.hpp>

import Memory;
import FormBuilder;

class [[nodiscard]] EditMetadataDialog final
    : protected ModernDialogInterface
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