#ifndef PROCESSOR_DISABLE
const char *get_enum___pid_type_str(int value)
{
	switch(value){
		case F_OWNER_TID: return "F_OWNER_TID";
		case F_OWNER_PID: return "F_OWNER_PID";
		case F_OWNER_PGRP: return "F_OWNER_PGRP";
		case F_OWNER_GID: return "F_OWNER_GID";
		case F_OWNER_PGRP: return "F_OWNER_PGRP";
	}
}
const char *get_enum_ImGuiWindowFlags__str(int value)
{
	switch(value){
		case ImGuiWindowFlags_None: return "ImGuiWindowFlags_None";
		case ImGuiWindowFlags_NoTitleBar: return "ImGuiWindowFlags_NoTitleBar";
		case ImGuiWindowFlags_NoResize: return "ImGuiWindowFlags_NoResize";
		case ImGuiWindowFlags_NoMove: return "ImGuiWindowFlags_NoMove";
		case ImGuiWindowFlags_NoScrollbar: return "ImGuiWindowFlags_NoScrollbar";
		case ImGuiWindowFlags_NoScrollWithMouse: return "ImGuiWindowFlags_NoScrollWithMouse";
		case ImGuiWindowFlags_NoCollapse: return "ImGuiWindowFlags_NoCollapse";
		case ImGuiWindowFlags_AlwaysAutoResize: return "ImGuiWindowFlags_AlwaysAutoResize";
		case ImGuiWindowFlags_NoBackground: return "ImGuiWindowFlags_NoBackground";
		case ImGuiWindowFlags_NoSavedSettings: return "ImGuiWindowFlags_NoSavedSettings";
		case ImGuiWindowFlags_NoMouseInputs: return "ImGuiWindowFlags_NoMouseInputs";
		case ImGuiWindowFlags_MenuBar: return "ImGuiWindowFlags_MenuBar";
		case ImGuiWindowFlags_HorizontalScrollbar: return "ImGuiWindowFlags_HorizontalScrollbar";
		case ImGuiWindowFlags_NoFocusOnAppearing: return "ImGuiWindowFlags_NoFocusOnAppearing";
		case ImGuiWindowFlags_NoBringToFrontOnFocus: return "ImGuiWindowFlags_NoBringToFrontOnFocus";
		case ImGuiWindowFlags_AlwaysVerticalScrollbar: return "ImGuiWindowFlags_AlwaysVerticalScrollbar";
		case ImGuiWindowFlags_AlwaysHorizontalScrollbar: return "ImGuiWindowFlags_AlwaysHorizontalScrollbar";
		case ImGuiWindowFlags_NoNavInputs: return "ImGuiWindowFlags_NoNavInputs";
		case ImGuiWindowFlags_NoNavFocus: return "ImGuiWindowFlags_NoNavFocus";
		case ImGuiWindowFlags_UnsavedDocument: return "ImGuiWindowFlags_UnsavedDocument";
		case ImGuiWindowFlags_NoNav: return "ImGuiWindowFlags_NoNav";
		case ImGuiWindowFlags_NoNavInputs: return "ImGuiWindowFlags_NoNavInputs";
		case ImGuiWindowFlags_NoNavFocus: return "ImGuiWindowFlags_NoNavFocus";
		case ImGuiWindowFlags_NoDecoration: return "ImGuiWindowFlags_NoDecoration";
		case ImGuiWindowFlags_NoTitleBar: return "ImGuiWindowFlags_NoTitleBar";
		case ImGuiWindowFlags_NoResize: return "ImGuiWindowFlags_NoResize";
		case ImGuiWindowFlags_NoScrollbar: return "ImGuiWindowFlags_NoScrollbar";
		case ImGuiWindowFlags_NoCollapse: return "ImGuiWindowFlags_NoCollapse";
		case ImGuiWindowFlags_NoInputs: return "ImGuiWindowFlags_NoInputs";
		case ImGuiWindowFlags_NoMouseInputs: return "ImGuiWindowFlags_NoMouseInputs";
		case ImGuiWindowFlags_NoNavInputs: return "ImGuiWindowFlags_NoNavInputs";
		case ImGuiWindowFlags_NoNavFocus: return "ImGuiWindowFlags_NoNavFocus";
		case ImGuiWindowFlags_NavFlattened: return "ImGuiWindowFlags_NavFlattened";
		case ImGuiWindowFlags_ChildWindow: return "ImGuiWindowFlags_ChildWindow";
		case ImGuiWindowFlags_Tooltip: return "ImGuiWindowFlags_Tooltip";
		case ImGuiWindowFlags_Popup: return "ImGuiWindowFlags_Popup";
		case ImGuiWindowFlags_Modal: return "ImGuiWindowFlags_Modal";
		case ImGuiWindowFlags_ChildMenu: return "ImGuiWindowFlags_ChildMenu";
		case ImGuiWindowFlags_AlwaysUseWindowPadding: return "ImGuiWindowFlags_AlwaysUseWindowPadding";
	}
}
const char *get_enum_ImGuiChildFlags__str(int value)
{
	switch(value){
		case ImGuiChildFlags_None: return "ImGuiChildFlags_None";
		case ImGuiChildFlags_Border: return "ImGuiChildFlags_Border";
		case ImGuiChildFlags_AlwaysUseWindowPadding: return "ImGuiChildFlags_AlwaysUseWindowPadding";
		case ImGuiChildFlags_ResizeX: return "ImGuiChildFlags_ResizeX";
		case ImGuiChildFlags_ResizeY: return "ImGuiChildFlags_ResizeY";
		case ImGuiChildFlags_AutoResizeX: return "ImGuiChildFlags_AutoResizeX";
		case ImGuiChildFlags_AutoResizeY: return "ImGuiChildFlags_AutoResizeY";
		case ImGuiChildFlags_AlwaysAutoResize: return "ImGuiChildFlags_AlwaysAutoResize";
		case ImGuiChildFlags_FrameStyle: return "ImGuiChildFlags_FrameStyle";
	}
}
const char *get_enum_ImGuiInputTextFlags__str(int value)
{
	switch(value){
		case ImGuiInputTextFlags_None: return "ImGuiInputTextFlags_None";
		case ImGuiInputTextFlags_CharsDecimal: return "ImGuiInputTextFlags_CharsDecimal";
		case ImGuiInputTextFlags_CharsHexadecimal: return "ImGuiInputTextFlags_CharsHexadecimal";
		case ImGuiInputTextFlags_CharsUppercase: return "ImGuiInputTextFlags_CharsUppercase";
		case ImGuiInputTextFlags_CharsNoBlank: return "ImGuiInputTextFlags_CharsNoBlank";
		case ImGuiInputTextFlags_AutoSelectAll: return "ImGuiInputTextFlags_AutoSelectAll";
		case ImGuiInputTextFlags_EnterReturnsTrue: return "ImGuiInputTextFlags_EnterReturnsTrue";
		case ImGuiInputTextFlags_CallbackCompletion: return "ImGuiInputTextFlags_CallbackCompletion";
		case ImGuiInputTextFlags_CallbackHistory: return "ImGuiInputTextFlags_CallbackHistory";
		case ImGuiInputTextFlags_CallbackAlways: return "ImGuiInputTextFlags_CallbackAlways";
		case ImGuiInputTextFlags_CallbackCharFilter: return "ImGuiInputTextFlags_CallbackCharFilter";
		case ImGuiInputTextFlags_AllowTabInput: return "ImGuiInputTextFlags_AllowTabInput";
		case ImGuiInputTextFlags_CtrlEnterForNewLine: return "ImGuiInputTextFlags_CtrlEnterForNewLine";
		case ImGuiInputTextFlags_NoHorizontalScroll: return "ImGuiInputTextFlags_NoHorizontalScroll";
		case ImGuiInputTextFlags_AlwaysOverwrite: return "ImGuiInputTextFlags_AlwaysOverwrite";
		case ImGuiInputTextFlags_ReadOnly: return "ImGuiInputTextFlags_ReadOnly";
		case ImGuiInputTextFlags_Password: return "ImGuiInputTextFlags_Password";
		case ImGuiInputTextFlags_NoUndoRedo: return "ImGuiInputTextFlags_NoUndoRedo";
		case ImGuiInputTextFlags_CharsScientific: return "ImGuiInputTextFlags_CharsScientific";
		case ImGuiInputTextFlags_CallbackResize: return "ImGuiInputTextFlags_CallbackResize";
		case ImGuiInputTextFlags_CallbackEdit: return "ImGuiInputTextFlags_CallbackEdit";
		case ImGuiInputTextFlags_EscapeClearsAll: return "ImGuiInputTextFlags_EscapeClearsAll";
	}
}
const char *get_enum_ImGuiTreeNodeFlags__str(int value)
{
	switch(value){
		case ImGuiTreeNodeFlags_None: return "ImGuiTreeNodeFlags_None";
		case ImGuiTreeNodeFlags_Selected: return "ImGuiTreeNodeFlags_Selected";
		case ImGuiTreeNodeFlags_Framed: return "ImGuiTreeNodeFlags_Framed";
		case ImGuiTreeNodeFlags_AllowOverlap: return "ImGuiTreeNodeFlags_AllowOverlap";
		case ImGuiTreeNodeFlags_NoTreePushOnOpen: return "ImGuiTreeNodeFlags_NoTreePushOnOpen";
		case ImGuiTreeNodeFlags_NoAutoOpenOnLog: return "ImGuiTreeNodeFlags_NoAutoOpenOnLog";
		case ImGuiTreeNodeFlags_DefaultOpen: return "ImGuiTreeNodeFlags_DefaultOpen";
		case ImGuiTreeNodeFlags_OpenOnDoubleClick: return "ImGuiTreeNodeFlags_OpenOnDoubleClick";
		case ImGuiTreeNodeFlags_OpenOnArrow: return "ImGuiTreeNodeFlags_OpenOnArrow";
		case ImGuiTreeNodeFlags_Leaf: return "ImGuiTreeNodeFlags_Leaf";
		case ImGuiTreeNodeFlags_Bullet: return "ImGuiTreeNodeFlags_Bullet";
		case ImGuiTreeNodeFlags_FramePadding: return "ImGuiTreeNodeFlags_FramePadding";
		case ImGuiTreeNodeFlags_SpanAvailWidth: return "ImGuiTreeNodeFlags_SpanAvailWidth";
		case ImGuiTreeNodeFlags_SpanFullWidth: return "ImGuiTreeNodeFlags_SpanFullWidth";
		case ImGuiTreeNodeFlags_SpanAllColumns: return "ImGuiTreeNodeFlags_SpanAllColumns";
		case ImGuiTreeNodeFlags_NavLeftJumpsBackHere: return "ImGuiTreeNodeFlags_NavLeftJumpsBackHere";
		case ImGuiTreeNodeFlags_CollapsingHeader: return "ImGuiTreeNodeFlags_CollapsingHeader";
		case ImGuiTreeNodeFlags_Framed: return "ImGuiTreeNodeFlags_Framed";
		case ImGuiTreeNodeFlags_NoTreePushOnOpen: return "ImGuiTreeNodeFlags_NoTreePushOnOpen";
		case ImGuiTreeNodeFlags_NoAutoOpenOnLog: return "ImGuiTreeNodeFlags_NoAutoOpenOnLog";
		case ImGuiTreeNodeFlags_AllowItemOverlap: return "ImGuiTreeNodeFlags_AllowItemOverlap";
		case ImGuiTreeNodeFlags_AllowOverlap: return "ImGuiTreeNodeFlags_AllowOverlap";
	}
}
const char *get_enum_ImGuiPopupFlags__str(int value)
{
	switch(value){
		case ImGuiPopupFlags_None: return "ImGuiPopupFlags_None";
		case ImGuiPopupFlags_MouseButtonLeft: return "ImGuiPopupFlags_MouseButtonLeft";
		case ImGuiPopupFlags_MouseButtonRight: return "ImGuiPopupFlags_MouseButtonRight";
		case ImGuiPopupFlags_MouseButtonMiddle: return "ImGuiPopupFlags_MouseButtonMiddle";
		case ImGuiPopupFlags_MouseButtonMask_: return "ImGuiPopupFlags_MouseButtonMask_";
		case x1F: return "x1F";
		case ImGuiPopupFlags_MouseButtonDefault_: return "ImGuiPopupFlags_MouseButtonDefault_";
		case ImGuiPopupFlags_NoOpenOverExistingPopup: return "ImGuiPopupFlags_NoOpenOverExistingPopup";
		case ImGuiPopupFlags_NoOpenOverItems: return "ImGuiPopupFlags_NoOpenOverItems";
		case ImGuiPopupFlags_AnyPopupId: return "ImGuiPopupFlags_AnyPopupId";
		case ImGuiPopupFlags_AnyPopupLevel: return "ImGuiPopupFlags_AnyPopupLevel";
		case ImGuiPopupFlags_AnyPopup: return "ImGuiPopupFlags_AnyPopup";
		case ImGuiPopupFlags_AnyPopupId: return "ImGuiPopupFlags_AnyPopupId";
		case ImGuiPopupFlags_AnyPopupLevel: return "ImGuiPopupFlags_AnyPopupLevel";
	}
}
const char *get_enum_ImGuiSelectableFlags__str(int value)
{
	switch(value){
		case ImGuiSelectableFlags_None: return "ImGuiSelectableFlags_None";
		case ImGuiSelectableFlags_DontClosePopups: return "ImGuiSelectableFlags_DontClosePopups";
		case ImGuiSelectableFlags_SpanAllColumns: return "ImGuiSelectableFlags_SpanAllColumns";
		case ImGuiSelectableFlags_AllowDoubleClick: return "ImGuiSelectableFlags_AllowDoubleClick";
		case ImGuiSelectableFlags_Disabled: return "ImGuiSelectableFlags_Disabled";
		case ImGuiSelectableFlags_AllowOverlap: return "ImGuiSelectableFlags_AllowOverlap";
		case ImGuiSelectableFlags_AllowItemOverlap: return "ImGuiSelectableFlags_AllowItemOverlap";
		case ImGuiSelectableFlags_AllowOverlap: return "ImGuiSelectableFlags_AllowOverlap";
	}
}
const char *get_enum_ImGuiComboFlags__str(int value)
{
	switch(value){
		case ImGuiComboFlags_None: return "ImGuiComboFlags_None";
		case ImGuiComboFlags_PopupAlignLeft: return "ImGuiComboFlags_PopupAlignLeft";
		case ImGuiComboFlags_HeightSmall: return "ImGuiComboFlags_HeightSmall";
		case ImGuiComboFlags_HeightRegular: return "ImGuiComboFlags_HeightRegular";
		case ImGuiComboFlags_HeightLarge: return "ImGuiComboFlags_HeightLarge";
		case ImGuiComboFlags_HeightLargest: return "ImGuiComboFlags_HeightLargest";
		case ImGuiComboFlags_NoArrowButton: return "ImGuiComboFlags_NoArrowButton";
		case ImGuiComboFlags_NoPreview: return "ImGuiComboFlags_NoPreview";
		case ImGuiComboFlags_WidthFitPreview: return "ImGuiComboFlags_WidthFitPreview";
		case ImGuiComboFlags_HeightMask_: return "ImGuiComboFlags_HeightMask_";
		case ImGuiComboFlags_HeightSmall: return "ImGuiComboFlags_HeightSmall";
		case ImGuiComboFlags_HeightRegular: return "ImGuiComboFlags_HeightRegular";
		case ImGuiComboFlags_HeightLarge: return "ImGuiComboFlags_HeightLarge";
		case ImGuiComboFlags_HeightLargest: return "ImGuiComboFlags_HeightLargest";
	}
}
const char *get_enum_ImGuiTabBarFlags__str(int value)
{
	switch(value){
		case ImGuiTabBarFlags_None: return "ImGuiTabBarFlags_None";
		case ImGuiTabBarFlags_Reorderable: return "ImGuiTabBarFlags_Reorderable";
		case ImGuiTabBarFlags_AutoSelectNewTabs: return "ImGuiTabBarFlags_AutoSelectNewTabs";
		case ImGuiTabBarFlags_TabListPopupButton: return "ImGuiTabBarFlags_TabListPopupButton";
		case ImGuiTabBarFlags_NoCloseWithMiddleMouseButton: return "ImGuiTabBarFlags_NoCloseWithMiddleMouseButton";
		case ImGuiTabBarFlags_NoTabListScrollingButtons: return "ImGuiTabBarFlags_NoTabListScrollingButtons";
		case ImGuiTabBarFlags_NoTooltip: return "ImGuiTabBarFlags_NoTooltip";
		case ImGuiTabBarFlags_FittingPolicyResizeDown: return "ImGuiTabBarFlags_FittingPolicyResizeDown";
		case ImGuiTabBarFlags_FittingPolicyScroll: return "ImGuiTabBarFlags_FittingPolicyScroll";
		case ImGuiTabBarFlags_FittingPolicyMask_: return "ImGuiTabBarFlags_FittingPolicyMask_";
		case ImGuiTabBarFlags_FittingPolicyResizeDown: return "ImGuiTabBarFlags_FittingPolicyResizeDown";
		case ImGuiTabBarFlags_FittingPolicyScroll: return "ImGuiTabBarFlags_FittingPolicyScroll";
		case ImGuiTabBarFlags_FittingPolicyDefault_: return "ImGuiTabBarFlags_FittingPolicyDefault_";
		case ImGuiTabBarFlags_FittingPolicyResizeDown: return "ImGuiTabBarFlags_FittingPolicyResizeDown";
	}
}
const char *get_enum_ImGuiTabItemFlags__str(int value)
{
	switch(value){
		case ImGuiTabItemFlags_None: return "ImGuiTabItemFlags_None";
		case ImGuiTabItemFlags_UnsavedDocument: return "ImGuiTabItemFlags_UnsavedDocument";
		case ImGuiTabItemFlags_SetSelected: return "ImGuiTabItemFlags_SetSelected";
		case ImGuiTabItemFlags_NoCloseWithMiddleMouseButton: return "ImGuiTabItemFlags_NoCloseWithMiddleMouseButton";
		case ImGuiTabItemFlags_NoPushId: return "ImGuiTabItemFlags_NoPushId";
		case ImGuiTabItemFlags_NoTooltip: return "ImGuiTabItemFlags_NoTooltip";
		case ImGuiTabItemFlags_NoReorder: return "ImGuiTabItemFlags_NoReorder";
		case ImGuiTabItemFlags_Leading: return "ImGuiTabItemFlags_Leading";
		case ImGuiTabItemFlags_Trailing: return "ImGuiTabItemFlags_Trailing";
		case ImGuiTabItemFlags_NoAssumedClosure: return "ImGuiTabItemFlags_NoAssumedClosure";
	}
}
const char *get_enum_ImGuiFocusedFlags__str(int value)
{
	switch(value){
		case ImGuiFocusedFlags_None: return "ImGuiFocusedFlags_None";
		case ImGuiFocusedFlags_ChildWindows: return "ImGuiFocusedFlags_ChildWindows";
		case ImGuiFocusedFlags_RootWindow: return "ImGuiFocusedFlags_RootWindow";
		case ImGuiFocusedFlags_AnyWindow: return "ImGuiFocusedFlags_AnyWindow";
		case ImGuiFocusedFlags_NoPopupHierarchy: return "ImGuiFocusedFlags_NoPopupHierarchy";
		case ImGuiFocusedFlags_RootAndChildWindows: return "ImGuiFocusedFlags_RootAndChildWindows";
		case ImGuiFocusedFlags_RootWindow: return "ImGuiFocusedFlags_RootWindow";
		case ImGuiFocusedFlags_ChildWindows: return "ImGuiFocusedFlags_ChildWindows";
	}
}
const char *get_enum_ImGuiHoveredFlags__str(int value)
{
	switch(value){
		case ImGuiHoveredFlags_None: return "ImGuiHoveredFlags_None";
		case ImGuiHoveredFlags_ChildWindows: return "ImGuiHoveredFlags_ChildWindows";
		case ImGuiHoveredFlags_RootWindow: return "ImGuiHoveredFlags_RootWindow";
		case ImGuiHoveredFlags_AnyWindow: return "ImGuiHoveredFlags_AnyWindow";
		case ImGuiHoveredFlags_NoPopupHierarchy: return "ImGuiHoveredFlags_NoPopupHierarchy";
		case ImGuiHoveredFlags_AllowWhenBlockedByPopup: return "ImGuiHoveredFlags_AllowWhenBlockedByPopup";
		case ImGuiHoveredFlags_AllowWhenBlockedByActiveItem: return "ImGuiHoveredFlags_AllowWhenBlockedByActiveItem";
		case ImGuiHoveredFlags_AllowWhenOverlappedByItem: return "ImGuiHoveredFlags_AllowWhenOverlappedByItem";
		case ImGuiHoveredFlags_AllowWhenOverlappedByWindow: return "ImGuiHoveredFlags_AllowWhenOverlappedByWindow";
		case ImGuiHoveredFlags_AllowWhenDisabled: return "ImGuiHoveredFlags_AllowWhenDisabled";
		case ImGuiHoveredFlags_NoNavOverride: return "ImGuiHoveredFlags_NoNavOverride";
		case ImGuiHoveredFlags_AllowWhenOverlapped: return "ImGuiHoveredFlags_AllowWhenOverlapped";
		case ImGuiHoveredFlags_AllowWhenOverlappedByItem: return "ImGuiHoveredFlags_AllowWhenOverlappedByItem";
		case ImGuiHoveredFlags_AllowWhenOverlappedByWindow: return "ImGuiHoveredFlags_AllowWhenOverlappedByWindow";
		case ImGuiHoveredFlags_RectOnly: return "ImGuiHoveredFlags_RectOnly";
		case ImGuiHoveredFlags_AllowWhenBlockedByPopup: return "ImGuiHoveredFlags_AllowWhenBlockedByPopup";
		case ImGuiHoveredFlags_AllowWhenBlockedByActiveItem: return "ImGuiHoveredFlags_AllowWhenBlockedByActiveItem";
		case ImGuiHoveredFlags_AllowWhenOverlapped: return "ImGuiHoveredFlags_AllowWhenOverlapped";
		case ImGuiHoveredFlags_RootAndChildWindows: return "ImGuiHoveredFlags_RootAndChildWindows";
		case ImGuiHoveredFlags_RootWindow: return "ImGuiHoveredFlags_RootWindow";
		case ImGuiHoveredFlags_ChildWindows: return "ImGuiHoveredFlags_ChildWindows";
		case ImGuiHoveredFlags_ForTooltip: return "ImGuiHoveredFlags_ForTooltip";
		case ImGuiHoveredFlags_Stationary: return "ImGuiHoveredFlags_Stationary";
		case ImGuiHoveredFlags_DelayNone: return "ImGuiHoveredFlags_DelayNone";
		case ImGuiHoveredFlags_DelayShort: return "ImGuiHoveredFlags_DelayShort";
		case ImGuiHoveredFlags_DelayNormal: return "ImGuiHoveredFlags_DelayNormal";
		case ImGuiHoveredFlags_NoSharedDelay: return "ImGuiHoveredFlags_NoSharedDelay";
	}
}
const char *get_enum_ImGuiDragDropFlags__str(int value)
{
	switch(value){
		case ImGuiDragDropFlags_None: return "ImGuiDragDropFlags_None";
		case ImGuiDragDropFlags_SourceNoPreviewTooltip: return "ImGuiDragDropFlags_SourceNoPreviewTooltip";
		case ImGuiDragDropFlags_SourceNoDisableHover: return "ImGuiDragDropFlags_SourceNoDisableHover";
		case ImGuiDragDropFlags_SourceNoHoldToOpenOthers: return "ImGuiDragDropFlags_SourceNoHoldToOpenOthers";
		case ImGuiDragDropFlags_SourceAllowNullID: return "ImGuiDragDropFlags_SourceAllowNullID";
		case ImGuiDragDropFlags_SourceExtern: return "ImGuiDragDropFlags_SourceExtern";
		case ImGuiDragDropFlags_SourceAutoExpirePayload: return "ImGuiDragDropFlags_SourceAutoExpirePayload";
		case ImGuiDragDropFlags_AcceptBeforeDelivery: return "ImGuiDragDropFlags_AcceptBeforeDelivery";
		case ImGuiDragDropFlags_AcceptNoDrawDefaultRect: return "ImGuiDragDropFlags_AcceptNoDrawDefaultRect";
		case ImGuiDragDropFlags_AcceptNoPreviewTooltip: return "ImGuiDragDropFlags_AcceptNoPreviewTooltip";
		case ImGuiDragDropFlags_AcceptPeekOnly: return "ImGuiDragDropFlags_AcceptPeekOnly";
		case ImGuiDragDropFlags_AcceptBeforeDelivery: return "ImGuiDragDropFlags_AcceptBeforeDelivery";
		case ImGuiDragDropFlags_AcceptNoDrawDefaultRect: return "ImGuiDragDropFlags_AcceptNoDrawDefaultRect";
	}
}
const char *get_enum_ImGuiDataType__str(int value)
{
	switch(value){
		case ImGuiDataType_S8: return "ImGuiDataType_S8";
		case ImGuiDataType_U8: return "ImGuiDataType_U8";
		case ImGuiDataType_S16: return "ImGuiDataType_S16";
		case ImGuiDataType_U16: return "ImGuiDataType_U16";
		case ImGuiDataType_S32: return "ImGuiDataType_S32";
		case ImGuiDataType_U32: return "ImGuiDataType_U32";
		case ImGuiDataType_S64: return "ImGuiDataType_S64";
		case ImGuiDataType_U64: return "ImGuiDataType_U64";
		case ImGuiDataType_Float: return "ImGuiDataType_Float";
		case ImGuiDataType_Double: return "ImGuiDataType_Double";
		case ImGuiDataType_COUNT: return "ImGuiDataType_COUNT";
	}
}
const char *get_enum_ImGuiDir__str(int value)
{
	switch(value){
		case ImGuiDir_None: return "ImGuiDir_None";
		case ImGuiDir_Left: return "ImGuiDir_Left";
		case ImGuiDir_Right: return "ImGuiDir_Right";
		case ImGuiDir_Up: return "ImGuiDir_Up";
		case ImGuiDir_Down: return "ImGuiDir_Down";
		case ImGuiDir_COUNT: return "ImGuiDir_COUNT";
	}
}
const char *get_enum_ImGuiSortDirection__str(int value)
{
	switch(value){
		case ImGuiSortDirection_None: return "ImGuiSortDirection_None";
		case ImGuiSortDirection_Ascending: return "ImGuiSortDirection_Ascending";
		case ImGuiSortDirection_Descending: return "ImGuiSortDirection_Descending";
	}
}
const char *get_enum_ImGuiNavInput_str(int value)
{
	switch(value){
		case ImGuiNavInput_Activate: return "ImGuiNavInput_Activate";
		case ImGuiNavInput_Cancel: return "ImGuiNavInput_Cancel";
		case ImGuiNavInput_Input: return "ImGuiNavInput_Input";
		case ImGuiNavInput_Menu: return "ImGuiNavInput_Menu";
		case ImGuiNavInput_DpadLeft: return "ImGuiNavInput_DpadLeft";
		case ImGuiNavInput_DpadRight: return "ImGuiNavInput_DpadRight";
		case ImGuiNavInput_DpadUp: return "ImGuiNavInput_DpadUp";
		case ImGuiNavInput_DpadDown: return "ImGuiNavInput_DpadDown";
		case ImGuiNavInput_LStickLeft: return "ImGuiNavInput_LStickLeft";
		case ImGuiNavInput_LStickRight: return "ImGuiNavInput_LStickRight";
		case ImGuiNavInput_LStickUp: return "ImGuiNavInput_LStickUp";
		case ImGuiNavInput_LStickDown: return "ImGuiNavInput_LStickDown";
		case ImGuiNavInput_FocusPrev: return "ImGuiNavInput_FocusPrev";
		case ImGuiNavInput_FocusNext: return "ImGuiNavInput_FocusNext";
		case ImGuiNavInput_TweakSlow: return "ImGuiNavInput_TweakSlow";
		case ImGuiNavInput_TweakFast: return "ImGuiNavInput_TweakFast";
		case ImGuiNavInput_COUNT: return "ImGuiNavInput_COUNT";
	}
}
const char *get_enum_ImGuiConfigFlags__str(int value)
{
	switch(value){
		case ImGuiConfigFlags_None: return "ImGuiConfigFlags_None";
		case ImGuiConfigFlags_NavEnableKeyboard: return "ImGuiConfigFlags_NavEnableKeyboard";
		case ImGuiConfigFlags_NavEnableGamepad: return "ImGuiConfigFlags_NavEnableGamepad";
		case ImGuiConfigFlags_NavEnableSetMousePos: return "ImGuiConfigFlags_NavEnableSetMousePos";
		case ImGuiConfigFlags_NavNoCaptureKeyboard: return "ImGuiConfigFlags_NavNoCaptureKeyboard";
		case ImGuiConfigFlags_NoMouse: return "ImGuiConfigFlags_NoMouse";
		case ImGuiConfigFlags_NoMouseCursorChange: return "ImGuiConfigFlags_NoMouseCursorChange";
		case ImGuiConfigFlags_IsSRGB: return "ImGuiConfigFlags_IsSRGB";
		case ImGuiConfigFlags_IsTouchScreen: return "ImGuiConfigFlags_IsTouchScreen";
	}
}
const char *get_enum_ImGuiBackendFlags__str(int value)
{
	switch(value){
		case ImGuiBackendFlags_None: return "ImGuiBackendFlags_None";
		case ImGuiBackendFlags_HasGamepad: return "ImGuiBackendFlags_HasGamepad";
		case ImGuiBackendFlags_HasMouseCursors: return "ImGuiBackendFlags_HasMouseCursors";
		case ImGuiBackendFlags_HasSetMousePos: return "ImGuiBackendFlags_HasSetMousePos";
		case ImGuiBackendFlags_RendererHasVtxOffset: return "ImGuiBackendFlags_RendererHasVtxOffset";
	}
}
const char *get_enum_ImGuiCol__str(int value)
{
	switch(value){
		case ImGuiCol_Text: return "ImGuiCol_Text";
		case ImGuiCol_TextDisabled: return "ImGuiCol_TextDisabled";
		case ImGuiCol_WindowBg: return "ImGuiCol_WindowBg";
		case ImGuiCol_ChildBg: return "ImGuiCol_ChildBg";
		case ImGuiCol_PopupBg: return "ImGuiCol_PopupBg";
		case ImGuiCol_Border: return "ImGuiCol_Border";
		case ImGuiCol_BorderShadow: return "ImGuiCol_BorderShadow";
		case ImGuiCol_FrameBg: return "ImGuiCol_FrameBg";
		case ImGuiCol_FrameBgHovered: return "ImGuiCol_FrameBgHovered";
		case ImGuiCol_FrameBgActive: return "ImGuiCol_FrameBgActive";
		case ImGuiCol_TitleBg: return "ImGuiCol_TitleBg";
		case ImGuiCol_TitleBgActive: return "ImGuiCol_TitleBgActive";
		case ImGuiCol_TitleBgCollapsed: return "ImGuiCol_TitleBgCollapsed";
		case ImGuiCol_MenuBarBg: return "ImGuiCol_MenuBarBg";
		case ImGuiCol_ScrollbarBg: return "ImGuiCol_ScrollbarBg";
		case ImGuiCol_ScrollbarGrab: return "ImGuiCol_ScrollbarGrab";
		case ImGuiCol_ScrollbarGrabHovered: return "ImGuiCol_ScrollbarGrabHovered";
		case ImGuiCol_ScrollbarGrabActive: return "ImGuiCol_ScrollbarGrabActive";
		case ImGuiCol_CheckMark: return "ImGuiCol_CheckMark";
		case ImGuiCol_SliderGrab: return "ImGuiCol_SliderGrab";
		case ImGuiCol_SliderGrabActive: return "ImGuiCol_SliderGrabActive";
		case ImGuiCol_Button: return "ImGuiCol_Button";
		case ImGuiCol_ButtonHovered: return "ImGuiCol_ButtonHovered";
		case ImGuiCol_ButtonActive: return "ImGuiCol_ButtonActive";
		case ImGuiCol_Header: return "ImGuiCol_Header";
		case ImGuiCol_HeaderHovered: return "ImGuiCol_HeaderHovered";
		case ImGuiCol_HeaderActive: return "ImGuiCol_HeaderActive";
		case ImGuiCol_Separator: return "ImGuiCol_Separator";
		case ImGuiCol_SeparatorHovered: return "ImGuiCol_SeparatorHovered";
		case ImGuiCol_SeparatorActive: return "ImGuiCol_SeparatorActive";
		case ImGuiCol_ResizeGrip: return "ImGuiCol_ResizeGrip";
		case ImGuiCol_ResizeGripHovered: return "ImGuiCol_ResizeGripHovered";
		case ImGuiCol_ResizeGripActive: return "ImGuiCol_ResizeGripActive";
		case ImGuiCol_Tab: return "ImGuiCol_Tab";
		case ImGuiCol_TabHovered: return "ImGuiCol_TabHovered";
		case ImGuiCol_TabActive: return "ImGuiCol_TabActive";
		case ImGuiCol_TabUnfocused: return "ImGuiCol_TabUnfocused";
		case ImGuiCol_TabUnfocusedActive: return "ImGuiCol_TabUnfocusedActive";
		case ImGuiCol_PlotLines: return "ImGuiCol_PlotLines";
		case ImGuiCol_PlotLinesHovered: return "ImGuiCol_PlotLinesHovered";
		case ImGuiCol_PlotHistogram: return "ImGuiCol_PlotHistogram";
		case ImGuiCol_PlotHistogramHovered: return "ImGuiCol_PlotHistogramHovered";
		case ImGuiCol_TableHeaderBg: return "ImGuiCol_TableHeaderBg";
		case ImGuiCol_TableBorderStrong: return "ImGuiCol_TableBorderStrong";
		case ImGuiCol_TableBorderLight: return "ImGuiCol_TableBorderLight";
		case ImGuiCol_TableRowBg: return "ImGuiCol_TableRowBg";
		case ImGuiCol_TableRowBgAlt: return "ImGuiCol_TableRowBgAlt";
		case ImGuiCol_TextSelectedBg: return "ImGuiCol_TextSelectedBg";
		case ImGuiCol_DragDropTarget: return "ImGuiCol_DragDropTarget";
		case ImGuiCol_NavHighlight: return "ImGuiCol_NavHighlight";
		case ImGuiCol_NavWindowingHighlight: return "ImGuiCol_NavWindowingHighlight";
		case ImGuiCol_NavWindowingDimBg: return "ImGuiCol_NavWindowingDimBg";
		case ImGuiCol_ModalWindowDimBg: return "ImGuiCol_ModalWindowDimBg";
		case ImGuiCol_COUNT: return "ImGuiCol_COUNT";
	}
}
const char *get_enum_ImGuiStyleVar__str(int value)
{
	switch(value){
		case ImGuiStyleVar_Alpha: return "ImGuiStyleVar_Alpha";
		case ImGuiStyleVar_DisabledAlpha: return "ImGuiStyleVar_DisabledAlpha";
		case ImGuiStyleVar_WindowPadding: return "ImGuiStyleVar_WindowPadding";
		case ImGuiStyleVar_WindowRounding: return "ImGuiStyleVar_WindowRounding";
		case ImGuiStyleVar_WindowBorderSize: return "ImGuiStyleVar_WindowBorderSize";
		case ImGuiStyleVar_WindowMinSize: return "ImGuiStyleVar_WindowMinSize";
		case ImGuiStyleVar_WindowTitleAlign: return "ImGuiStyleVar_WindowTitleAlign";
		case ImGuiStyleVar_ChildRounding: return "ImGuiStyleVar_ChildRounding";
		case ImGuiStyleVar_ChildBorderSize: return "ImGuiStyleVar_ChildBorderSize";
		case ImGuiStyleVar_PopupRounding: return "ImGuiStyleVar_PopupRounding";
		case ImGuiStyleVar_PopupBorderSize: return "ImGuiStyleVar_PopupBorderSize";
		case ImGuiStyleVar_FramePadding: return "ImGuiStyleVar_FramePadding";
		case ImGuiStyleVar_FrameRounding: return "ImGuiStyleVar_FrameRounding";
		case ImGuiStyleVar_FrameBorderSize: return "ImGuiStyleVar_FrameBorderSize";
		case ImGuiStyleVar_ItemSpacing: return "ImGuiStyleVar_ItemSpacing";
		case ImGuiStyleVar_ItemInnerSpacing: return "ImGuiStyleVar_ItemInnerSpacing";
		case ImGuiStyleVar_IndentSpacing: return "ImGuiStyleVar_IndentSpacing";
		case ImGuiStyleVar_CellPadding: return "ImGuiStyleVar_CellPadding";
		case ImGuiStyleVar_ScrollbarSize: return "ImGuiStyleVar_ScrollbarSize";
		case ImGuiStyleVar_ScrollbarRounding: return "ImGuiStyleVar_ScrollbarRounding";
		case ImGuiStyleVar_GrabMinSize: return "ImGuiStyleVar_GrabMinSize";
		case ImGuiStyleVar_GrabRounding: return "ImGuiStyleVar_GrabRounding";
		case ImGuiStyleVar_TabRounding: return "ImGuiStyleVar_TabRounding";
		case ImGuiStyleVar_TabBarBorderSize: return "ImGuiStyleVar_TabBarBorderSize";
		case ImGuiStyleVar_ButtonTextAlign: return "ImGuiStyleVar_ButtonTextAlign";
		case ImGuiStyleVar_SelectableTextAlign: return "ImGuiStyleVar_SelectableTextAlign";
		case ImGuiStyleVar_SeparatorTextBorderSize: return "ImGuiStyleVar_SeparatorTextBorderSize";
		case ImGuiStyleVar_SeparatorTextAlign: return "ImGuiStyleVar_SeparatorTextAlign";
		case ImGuiStyleVar_SeparatorTextPadding: return "ImGuiStyleVar_SeparatorTextPadding";
		case ImGuiStyleVar_COUNT: return "ImGuiStyleVar_COUNT";
	}
}
const char *get_enum_ImGuiButtonFlags__str(int value)
{
	switch(value){
		case ImGuiButtonFlags_None: return "ImGuiButtonFlags_None";
		case ImGuiButtonFlags_MouseButtonLeft: return "ImGuiButtonFlags_MouseButtonLeft";
		case ImGuiButtonFlags_MouseButtonRight: return "ImGuiButtonFlags_MouseButtonRight";
		case ImGuiButtonFlags_MouseButtonMiddle: return "ImGuiButtonFlags_MouseButtonMiddle";
		case ImGuiButtonFlags_MouseButtonMask_: return "ImGuiButtonFlags_MouseButtonMask_";
		case ImGuiButtonFlags_MouseButtonLeft: return "ImGuiButtonFlags_MouseButtonLeft";
		case ImGuiButtonFlags_MouseButtonRight: return "ImGuiButtonFlags_MouseButtonRight";
		case ImGuiButtonFlags_MouseButtonMiddle: return "ImGuiButtonFlags_MouseButtonMiddle";
		case ImGuiButtonFlags_MouseButtonDefault_: return "ImGuiButtonFlags_MouseButtonDefault_";
		case ImGuiButtonFlags_MouseButtonLeft: return "ImGuiButtonFlags_MouseButtonLeft";
	}
}
const char *get_enum_ImGuiColorEditFlags__str(int value)
{
	switch(value){
		case ImGuiColorEditFlags_None: return "ImGuiColorEditFlags_None";
		case ImGuiColorEditFlags_NoAlpha: return "ImGuiColorEditFlags_NoAlpha";
		case ImGuiColorEditFlags_NoPicker: return "ImGuiColorEditFlags_NoPicker";
		case ImGuiColorEditFlags_NoOptions: return "ImGuiColorEditFlags_NoOptions";
		case ImGuiColorEditFlags_NoSmallPreview: return "ImGuiColorEditFlags_NoSmallPreview";
		case ImGuiColorEditFlags_NoInputs: return "ImGuiColorEditFlags_NoInputs";
		case ImGuiColorEditFlags_NoTooltip: return "ImGuiColorEditFlags_NoTooltip";
		case ImGuiColorEditFlags_NoLabel: return "ImGuiColorEditFlags_NoLabel";
		case ImGuiColorEditFlags_NoSidePreview: return "ImGuiColorEditFlags_NoSidePreview";
		case ImGuiColorEditFlags_NoDragDrop: return "ImGuiColorEditFlags_NoDragDrop";
		case ImGuiColorEditFlags_NoBorder: return "ImGuiColorEditFlags_NoBorder";
		case ImGuiColorEditFlags_AlphaBar: return "ImGuiColorEditFlags_AlphaBar";
		case ImGuiColorEditFlags_AlphaPreview: return "ImGuiColorEditFlags_AlphaPreview";
		case ImGuiColorEditFlags_AlphaPreviewHalf: return "ImGuiColorEditFlags_AlphaPreviewHalf";
		case ImGuiColorEditFlags_HDR: return "ImGuiColorEditFlags_HDR";
		case ImGuiColorEditFlags_DisplayRGB: return "ImGuiColorEditFlags_DisplayRGB";
		case ImGuiColorEditFlags_DisplayHSV: return "ImGuiColorEditFlags_DisplayHSV";
		case ImGuiColorEditFlags_DisplayHex: return "ImGuiColorEditFlags_DisplayHex";
		case ImGuiColorEditFlags_Uint8: return "ImGuiColorEditFlags_Uint8";
		case ImGuiColorEditFlags_Float: return "ImGuiColorEditFlags_Float";
		case ImGuiColorEditFlags_PickerHueBar: return "ImGuiColorEditFlags_PickerHueBar";
		case ImGuiColorEditFlags_PickerHueWheel: return "ImGuiColorEditFlags_PickerHueWheel";
		case ImGuiColorEditFlags_InputRGB: return "ImGuiColorEditFlags_InputRGB";
		case ImGuiColorEditFlags_InputHSV: return "ImGuiColorEditFlags_InputHSV";
		case ImGuiColorEditFlags_DefaultOptions_: return "ImGuiColorEditFlags_DefaultOptions_";
		case ImGuiColorEditFlags_Uint8: return "ImGuiColorEditFlags_Uint8";
		case ImGuiColorEditFlags_DisplayRGB: return "ImGuiColorEditFlags_DisplayRGB";
		case ImGuiColorEditFlags_InputRGB: return "ImGuiColorEditFlags_InputRGB";
		case ImGuiColorEditFlags_PickerHueBar: return "ImGuiColorEditFlags_PickerHueBar";
		case ImGuiColorEditFlags_DisplayMask_: return "ImGuiColorEditFlags_DisplayMask_";
		case ImGuiColorEditFlags_DisplayRGB: return "ImGuiColorEditFlags_DisplayRGB";
		case ImGuiColorEditFlags_DisplayHSV: return "ImGuiColorEditFlags_DisplayHSV";
		case ImGuiColorEditFlags_DisplayHex: return "ImGuiColorEditFlags_DisplayHex";
		case ImGuiColorEditFlags_DataTypeMask_: return "ImGuiColorEditFlags_DataTypeMask_";
		case ImGuiColorEditFlags_Uint8: return "ImGuiColorEditFlags_Uint8";
		case ImGuiColorEditFlags_Float: return "ImGuiColorEditFlags_Float";
		case ImGuiColorEditFlags_PickerMask_: return "ImGuiColorEditFlags_PickerMask_";
		case ImGuiColorEditFlags_PickerHueWheel: return "ImGuiColorEditFlags_PickerHueWheel";
		case ImGuiColorEditFlags_PickerHueBar: return "ImGuiColorEditFlags_PickerHueBar";
		case ImGuiColorEditFlags_InputMask_: return "ImGuiColorEditFlags_InputMask_";
		case ImGuiColorEditFlags_InputRGB: return "ImGuiColorEditFlags_InputRGB";
		case ImGuiColorEditFlags_InputHSV: return "ImGuiColorEditFlags_InputHSV";
	}
}
const char *get_enum_ImGuiSliderFlags__str(int value)
{
	switch(value){
		case ImGuiSliderFlags_None: return "ImGuiSliderFlags_None";
		case ImGuiSliderFlags_AlwaysClamp: return "ImGuiSliderFlags_AlwaysClamp";
		case ImGuiSliderFlags_Logarithmic: return "ImGuiSliderFlags_Logarithmic";
		case ImGuiSliderFlags_NoRoundToFormat: return "ImGuiSliderFlags_NoRoundToFormat";
		case ImGuiSliderFlags_NoInput: return "ImGuiSliderFlags_NoInput";
		case ImGuiSliderFlags_InvalidMask_: return "ImGuiSliderFlags_InvalidMask_";
		case x7000000F: return "x7000000F";
	}
}
const char *get_enum_ImGuiMouseButton__str(int value)
{
	switch(value){
		case ImGuiMouseButton_Left: return "ImGuiMouseButton_Left";
		case ImGuiMouseButton_Right: return "ImGuiMouseButton_Right";
		case ImGuiMouseButton_Middle: return "ImGuiMouseButton_Middle";
		case ImGuiMouseButton_COUNT: return "ImGuiMouseButton_COUNT";
	}
}
const char *get_enum_ImGuiMouseCursor__str(int value)
{
	switch(value){
		case ImGuiMouseCursor_None: return "ImGuiMouseCursor_None";
		case ImGuiMouseCursor_Arrow: return "ImGuiMouseCursor_Arrow";
		case ImGuiMouseCursor_TextInput: return "ImGuiMouseCursor_TextInput";
		case ImGuiMouseCursor_ResizeAll: return "ImGuiMouseCursor_ResizeAll";
		case ImGuiMouseCursor_ResizeNS: return "ImGuiMouseCursor_ResizeNS";
		case ImGuiMouseCursor_ResizeEW: return "ImGuiMouseCursor_ResizeEW";
		case ImGuiMouseCursor_ResizeNESW: return "ImGuiMouseCursor_ResizeNESW";
		case ImGuiMouseCursor_ResizeNWSE: return "ImGuiMouseCursor_ResizeNWSE";
		case ImGuiMouseCursor_Hand: return "ImGuiMouseCursor_Hand";
		case ImGuiMouseCursor_NotAllowed: return "ImGuiMouseCursor_NotAllowed";
		case ImGuiMouseCursor_COUNT: return "ImGuiMouseCursor_COUNT";
	}
}
const char *get_enum_ImGuiCond__str(int value)
{
	switch(value){
		case ImGuiCond_None: return "ImGuiCond_None";
		case ImGuiCond_Always: return "ImGuiCond_Always";
		case ImGuiCond_Once: return "ImGuiCond_Once";
		case ImGuiCond_FirstUseEver: return "ImGuiCond_FirstUseEver";
		case ImGuiCond_Appearing: return "ImGuiCond_Appearing";
	}
}
const char *get_enum_ImGuiTableFlags__str(int value)
{
	switch(value){
		case ImGuiTableFlags_None: return "ImGuiTableFlags_None";
		case ImGuiTableFlags_Resizable: return "ImGuiTableFlags_Resizable";
		case ImGuiTableFlags_Reorderable: return "ImGuiTableFlags_Reorderable";
		case ImGuiTableFlags_Hideable: return "ImGuiTableFlags_Hideable";
		case ImGuiTableFlags_Sortable: return "ImGuiTableFlags_Sortable";
		case ImGuiTableFlags_NoSavedSettings: return "ImGuiTableFlags_NoSavedSettings";
		case ImGuiTableFlags_ContextMenuInBody: return "ImGuiTableFlags_ContextMenuInBody";
		case ImGuiTableFlags_RowBg: return "ImGuiTableFlags_RowBg";
		case ImGuiTableFlags_BordersInnerH: return "ImGuiTableFlags_BordersInnerH";
		case ImGuiTableFlags_BordersOuterH: return "ImGuiTableFlags_BordersOuterH";
		case ImGuiTableFlags_BordersInnerV: return "ImGuiTableFlags_BordersInnerV";
		case ImGuiTableFlags_BordersOuterV: return "ImGuiTableFlags_BordersOuterV";
		case ImGuiTableFlags_BordersH: return "ImGuiTableFlags_BordersH";
		case ImGuiTableFlags_BordersInnerH: return "ImGuiTableFlags_BordersInnerH";
		case ImGuiTableFlags_BordersOuterH: return "ImGuiTableFlags_BordersOuterH";
		case ImGuiTableFlags_BordersV: return "ImGuiTableFlags_BordersV";
		case ImGuiTableFlags_BordersInnerV: return "ImGuiTableFlags_BordersInnerV";
		case ImGuiTableFlags_BordersOuterV: return "ImGuiTableFlags_BordersOuterV";
		case ImGuiTableFlags_BordersInner: return "ImGuiTableFlags_BordersInner";
		case ImGuiTableFlags_BordersInnerV: return "ImGuiTableFlags_BordersInnerV";
		case ImGuiTableFlags_BordersInnerH: return "ImGuiTableFlags_BordersInnerH";
		case ImGuiTableFlags_BordersOuter: return "ImGuiTableFlags_BordersOuter";
		case ImGuiTableFlags_BordersOuterV: return "ImGuiTableFlags_BordersOuterV";
		case ImGuiTableFlags_BordersOuterH: return "ImGuiTableFlags_BordersOuterH";
		case ImGuiTableFlags_Borders: return "ImGuiTableFlags_Borders";
		case ImGuiTableFlags_BordersInner: return "ImGuiTableFlags_BordersInner";
		case ImGuiTableFlags_BordersOuter: return "ImGuiTableFlags_BordersOuter";
		case ImGuiTableFlags_NoBordersInBody: return "ImGuiTableFlags_NoBordersInBody";
		case ImGuiTableFlags_NoBordersInBodyUntilResize: return "ImGuiTableFlags_NoBordersInBodyUntilResize";
		case ImGuiTableFlags_SizingFixedFit: return "ImGuiTableFlags_SizingFixedFit";
		case ImGuiTableFlags_SizingFixedSame: return "ImGuiTableFlags_SizingFixedSame";
		case ImGuiTableFlags_SizingStretchProp: return "ImGuiTableFlags_SizingStretchProp";
		case ImGuiTableFlags_SizingStretchSame: return "ImGuiTableFlags_SizingStretchSame";
		case ImGuiTableFlags_NoHostExtendX: return "ImGuiTableFlags_NoHostExtendX";
		case ImGuiTableFlags_NoHostExtendY: return "ImGuiTableFlags_NoHostExtendY";
		case ImGuiTableFlags_NoKeepColumnsVisible: return "ImGuiTableFlags_NoKeepColumnsVisible";
		case ImGuiTableFlags_PreciseWidths: return "ImGuiTableFlags_PreciseWidths";
		case ImGuiTableFlags_NoClip: return "ImGuiTableFlags_NoClip";
		case ImGuiTableFlags_PadOuterX: return "ImGuiTableFlags_PadOuterX";
		case ImGuiTableFlags_NoPadOuterX: return "ImGuiTableFlags_NoPadOuterX";
		case ImGuiTableFlags_NoPadInnerX: return "ImGuiTableFlags_NoPadInnerX";
		case ImGuiTableFlags_ScrollX: return "ImGuiTableFlags_ScrollX";
		case ImGuiTableFlags_ScrollY: return "ImGuiTableFlags_ScrollY";
		case ImGuiTableFlags_SortMulti: return "ImGuiTableFlags_SortMulti";
		case ImGuiTableFlags_SortTristate: return "ImGuiTableFlags_SortTristate";
		case ImGuiTableFlags_HighlightHoveredColumn: return "ImGuiTableFlags_HighlightHoveredColumn";
		case ImGuiTableFlags_SizingMask_: return "ImGuiTableFlags_SizingMask_";
		case ImGuiTableFlags_SizingFixedFit: return "ImGuiTableFlags_SizingFixedFit";
		case ImGuiTableFlags_SizingFixedSame: return "ImGuiTableFlags_SizingFixedSame";
		case ImGuiTableFlags_SizingStretchProp: return "ImGuiTableFlags_SizingStretchProp";
		case ImGuiTableFlags_SizingStretchSame: return "ImGuiTableFlags_SizingStretchSame";
	}
}
const char *get_enum_ImGuiTableColumnFlags__str(int value)
{
	switch(value){
		case ImGuiTableColumnFlags_None: return "ImGuiTableColumnFlags_None";
		case ImGuiTableColumnFlags_Disabled: return "ImGuiTableColumnFlags_Disabled";
		case ImGuiTableColumnFlags_DefaultHide: return "ImGuiTableColumnFlags_DefaultHide";
		case ImGuiTableColumnFlags_DefaultSort: return "ImGuiTableColumnFlags_DefaultSort";
		case ImGuiTableColumnFlags_WidthStretch: return "ImGuiTableColumnFlags_WidthStretch";
		case ImGuiTableColumnFlags_WidthFixed: return "ImGuiTableColumnFlags_WidthFixed";
		case ImGuiTableColumnFlags_NoResize: return "ImGuiTableColumnFlags_NoResize";
		case ImGuiTableColumnFlags_NoReorder: return "ImGuiTableColumnFlags_NoReorder";
		case ImGuiTableColumnFlags_NoHide: return "ImGuiTableColumnFlags_NoHide";
		case ImGuiTableColumnFlags_NoClip: return "ImGuiTableColumnFlags_NoClip";
		case ImGuiTableColumnFlags_NoSort: return "ImGuiTableColumnFlags_NoSort";
		case ImGuiTableColumnFlags_NoSortAscending: return "ImGuiTableColumnFlags_NoSortAscending";
		case ImGuiTableColumnFlags_NoSortDescending: return "ImGuiTableColumnFlags_NoSortDescending";
		case ImGuiTableColumnFlags_NoHeaderLabel: return "ImGuiTableColumnFlags_NoHeaderLabel";
		case ImGuiTableColumnFlags_NoHeaderWidth: return "ImGuiTableColumnFlags_NoHeaderWidth";
		case ImGuiTableColumnFlags_PreferSortAscending: return "ImGuiTableColumnFlags_PreferSortAscending";
		case ImGuiTableColumnFlags_PreferSortDescending: return "ImGuiTableColumnFlags_PreferSortDescending";
		case ImGuiTableColumnFlags_IndentEnable: return "ImGuiTableColumnFlags_IndentEnable";
		case ImGuiTableColumnFlags_IndentDisable: return "ImGuiTableColumnFlags_IndentDisable";
		case ImGuiTableColumnFlags_AngledHeader: return "ImGuiTableColumnFlags_AngledHeader";
		case ImGuiTableColumnFlags_IsEnabled: return "ImGuiTableColumnFlags_IsEnabled";
		case ImGuiTableColumnFlags_IsVisible: return "ImGuiTableColumnFlags_IsVisible";
		case ImGuiTableColumnFlags_IsSorted: return "ImGuiTableColumnFlags_IsSorted";
		case ImGuiTableColumnFlags_IsHovered: return "ImGuiTableColumnFlags_IsHovered";
		case ImGuiTableColumnFlags_WidthMask_: return "ImGuiTableColumnFlags_WidthMask_";
		case ImGuiTableColumnFlags_WidthStretch: return "ImGuiTableColumnFlags_WidthStretch";
		case ImGuiTableColumnFlags_WidthFixed: return "ImGuiTableColumnFlags_WidthFixed";
		case ImGuiTableColumnFlags_IndentMask_: return "ImGuiTableColumnFlags_IndentMask_";
		case ImGuiTableColumnFlags_IndentEnable: return "ImGuiTableColumnFlags_IndentEnable";
		case ImGuiTableColumnFlags_IndentDisable: return "ImGuiTableColumnFlags_IndentDisable";
		case ImGuiTableColumnFlags_StatusMask_: return "ImGuiTableColumnFlags_StatusMask_";
		case ImGuiTableColumnFlags_IsEnabled: return "ImGuiTableColumnFlags_IsEnabled";
		case ImGuiTableColumnFlags_IsVisible: return "ImGuiTableColumnFlags_IsVisible";
		case ImGuiTableColumnFlags_IsSorted: return "ImGuiTableColumnFlags_IsSorted";
		case ImGuiTableColumnFlags_IsHovered: return "ImGuiTableColumnFlags_IsHovered";
		case ImGuiTableColumnFlags_NoDirectResize_: return "ImGuiTableColumnFlags_NoDirectResize_";
	}
}
const char *get_enum_ImGuiTableRowFlags__str(int value)
{
	switch(value){
		case ImGuiTableRowFlags_None: return "ImGuiTableRowFlags_None";
		case ImGuiTableRowFlags_Headers: return "ImGuiTableRowFlags_Headers";
	}
}
const char *get_enum_ImGuiTableBgTarget__str(int value)
{
	switch(value){
		case ImGuiTableBgTarget_None: return "ImGuiTableBgTarget_None";
		case ImGuiTableBgTarget_RowBg0: return "ImGuiTableBgTarget_RowBg0";
		case ImGuiTableBgTarget_RowBg1: return "ImGuiTableBgTarget_RowBg1";
		case ImGuiTableBgTarget_CellBg: return "ImGuiTableBgTarget_CellBg";
	}
}
const char *get_enum_ImDrawFlags__str(int value)
{
	switch(value){
		case ImDrawFlags_None: return "ImDrawFlags_None";
		case ImDrawFlags_Closed: return "ImDrawFlags_Closed";
		case ImDrawFlags_RoundCornersTopLeft: return "ImDrawFlags_RoundCornersTopLeft";
		case ImDrawFlags_RoundCornersTopRight: return "ImDrawFlags_RoundCornersTopRight";
		case ImDrawFlags_RoundCornersBottomLeft: return "ImDrawFlags_RoundCornersBottomLeft";
		case ImDrawFlags_RoundCornersBottomRight: return "ImDrawFlags_RoundCornersBottomRight";
		case ImDrawFlags_RoundCornersNone: return "ImDrawFlags_RoundCornersNone";
		case ImDrawFlags_RoundCornersTop: return "ImDrawFlags_RoundCornersTop";
		case ImDrawFlags_RoundCornersTopLeft: return "ImDrawFlags_RoundCornersTopLeft";
		case ImDrawFlags_RoundCornersTopRight: return "ImDrawFlags_RoundCornersTopRight";
		case ImDrawFlags_RoundCornersBottom: return "ImDrawFlags_RoundCornersBottom";
		case ImDrawFlags_RoundCornersBottomLeft: return "ImDrawFlags_RoundCornersBottomLeft";
		case ImDrawFlags_RoundCornersBottomRight: return "ImDrawFlags_RoundCornersBottomRight";
		case ImDrawFlags_RoundCornersLeft: return "ImDrawFlags_RoundCornersLeft";
		case ImDrawFlags_RoundCornersBottomLeft: return "ImDrawFlags_RoundCornersBottomLeft";
		case ImDrawFlags_RoundCornersTopLeft: return "ImDrawFlags_RoundCornersTopLeft";
		case ImDrawFlags_RoundCornersRight: return "ImDrawFlags_RoundCornersRight";
		case ImDrawFlags_RoundCornersBottomRight: return "ImDrawFlags_RoundCornersBottomRight";
		case ImDrawFlags_RoundCornersTopRight: return "ImDrawFlags_RoundCornersTopRight";
		case ImDrawFlags_RoundCornersAll: return "ImDrawFlags_RoundCornersAll";
		case ImDrawFlags_RoundCornersTopLeft: return "ImDrawFlags_RoundCornersTopLeft";
		case ImDrawFlags_RoundCornersTopRight: return "ImDrawFlags_RoundCornersTopRight";
		case ImDrawFlags_RoundCornersBottomLeft: return "ImDrawFlags_RoundCornersBottomLeft";
		case ImDrawFlags_RoundCornersBottomRight: return "ImDrawFlags_RoundCornersBottomRight";
		case ImDrawFlags_RoundCornersDefault_: return "ImDrawFlags_RoundCornersDefault_";
		case ImDrawFlags_RoundCornersAll: return "ImDrawFlags_RoundCornersAll";
		case ImDrawFlags_RoundCornersMask_: return "ImDrawFlags_RoundCornersMask_";
		case ImDrawFlags_RoundCornersAll: return "ImDrawFlags_RoundCornersAll";
		case ImDrawFlags_RoundCornersNone: return "ImDrawFlags_RoundCornersNone";
	}
}
const char *get_enum_ImDrawListFlags__str(int value)
{
	switch(value){
		case ImDrawListFlags_None: return "ImDrawListFlags_None";
		case ImDrawListFlags_AntiAliasedLines: return "ImDrawListFlags_AntiAliasedLines";
		case ImDrawListFlags_AntiAliasedLinesUseTex: return "ImDrawListFlags_AntiAliasedLinesUseTex";
		case ImDrawListFlags_AntiAliasedFill: return "ImDrawListFlags_AntiAliasedFill";
		case ImDrawListFlags_AllowVtxOffset: return "ImDrawListFlags_AllowVtxOffset";
	}
}
const char *get_enum_ImFontAtlasFlags__str(int value)
{
	switch(value){
		case ImFontAtlasFlags_None: return "ImFontAtlasFlags_None";
		case ImFontAtlasFlags_NoPowerOfTwoHeight: return "ImFontAtlasFlags_NoPowerOfTwoHeight";
		case ImFontAtlasFlags_NoMouseCursors: return "ImFontAtlasFlags_NoMouseCursors";
		case ImFontAtlasFlags_NoBakedLines: return "ImFontAtlasFlags_NoBakedLines";
	}
}
const char *get_enum_ImGuiViewportFlags__str(int value)
{
	switch(value){
		case ImGuiViewportFlags_None: return "ImGuiViewportFlags_None";
		case ImGuiViewportFlags_IsPlatformWindow: return "ImGuiViewportFlags_IsPlatformWindow";
		case ImGuiViewportFlags_IsPlatformMonitor: return "ImGuiViewportFlags_IsPlatformMonitor";
		case ImGuiViewportFlags_OwnedByApp: return "ImGuiViewportFlags_OwnedByApp";
	}
}
const char *get_enum_ImGuiModFlags__str(int value)
{
	switch(value){
		case ImGuiModFlags_None: return "ImGuiModFlags_None";
		case ImGuiModFlags_Ctrl: return "ImGuiModFlags_Ctrl";
		case ImGuiMod_Ctrl: return "ImGuiMod_Ctrl";
		case ImGuiModFlags_Shift: return "ImGuiModFlags_Shift";
		case ImGuiMod_Shift: return "ImGuiMod_Shift";
		case ImGuiModFlags_Alt: return "ImGuiModFlags_Alt";
		case ImGuiMod_Alt: return "ImGuiMod_Alt";
		case ImGuiModFlags_Super: return "ImGuiModFlags_Super";
		case ImGuiMod_Super: return "ImGuiMod_Super";
	}
}
const char *get_enum_ufbx_rotation_order_str(int value)
{
	switch(value){
		case UFBX_ROTATION_ORDER_XYZ: return "UFBX_ROTATION_ORDER_XYZ";
		case UFBX_ROTATION_ORDER_XZY: return "UFBX_ROTATION_ORDER_XZY";
		case UFBX_ROTATION_ORDER_YZX: return "UFBX_ROTATION_ORDER_YZX";
		case UFBX_ROTATION_ORDER_YXZ: return "UFBX_ROTATION_ORDER_YXZ";
		case UFBX_ROTATION_ORDER_ZXY: return "UFBX_ROTATION_ORDER_ZXY";
		case UFBX_ROTATION_ORDER_ZYX: return "UFBX_ROTATION_ORDER_ZYX";
		case UFBX_ROTATION_ORDER_SPHERIC: return "UFBX_ROTATION_ORDER_SPHERIC";
		case UFBX_ROTATION_ORDER_FORCE_32BIT: return "UFBX_ROTATION_ORDER_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_dom_value_type_str(int value)
{
	switch(value){
		case UFBX_DOM_VALUE_NUMBER: return "UFBX_DOM_VALUE_NUMBER";
		case UFBX_DOM_VALUE_STRING: return "UFBX_DOM_VALUE_STRING";
		case UFBX_DOM_VALUE_ARRAY_I8: return "UFBX_DOM_VALUE_ARRAY_I8";
		case UFBX_DOM_VALUE_ARRAY_I32: return "UFBX_DOM_VALUE_ARRAY_I32";
		case UFBX_DOM_VALUE_ARRAY_I64: return "UFBX_DOM_VALUE_ARRAY_I64";
		case UFBX_DOM_VALUE_ARRAY_F32: return "UFBX_DOM_VALUE_ARRAY_F32";
		case UFBX_DOM_VALUE_ARRAY_F64: return "UFBX_DOM_VALUE_ARRAY_F64";
		case UFBX_DOM_VALUE_ARRAY_RAW_STRING: return "UFBX_DOM_VALUE_ARRAY_RAW_STRING";
		case UFBX_DOM_VALUE_ARRAY_IGNORED: return "UFBX_DOM_VALUE_ARRAY_IGNORED";
		case UFBX_DOM_VALUE_TYPE_FORCE_32BIT: return "UFBX_DOM_VALUE_TYPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_prop_type_str(int value)
{
	switch(value){
		case UFBX_PROP_UNKNOWN: return "UFBX_PROP_UNKNOWN";
		case UFBX_PROP_BOOLEAN: return "UFBX_PROP_BOOLEAN";
		case UFBX_PROP_INTEGER: return "UFBX_PROP_INTEGER";
		case UFBX_PROP_NUMBER: return "UFBX_PROP_NUMBER";
		case UFBX_PROP_VECTOR: return "UFBX_PROP_VECTOR";
		case UFBX_PROP_COLOR: return "UFBX_PROP_COLOR";
		case UFBX_PROP_COLOR_WITH_ALPHA: return "UFBX_PROP_COLOR_WITH_ALPHA";
		case UFBX_PROP_STRING: return "UFBX_PROP_STRING";
		case UFBX_PROP_DATE_TIME: return "UFBX_PROP_DATE_TIME";
		case UFBX_PROP_TRANSLATION: return "UFBX_PROP_TRANSLATION";
		case UFBX_PROP_ROTATION: return "UFBX_PROP_ROTATION";
		case UFBX_PROP_SCALING: return "UFBX_PROP_SCALING";
		case UFBX_PROP_DISTANCE: return "UFBX_PROP_DISTANCE";
		case UFBX_PROP_COMPOUND: return "UFBX_PROP_COMPOUND";
		case UFBX_PROP_BLOB: return "UFBX_PROP_BLOB";
		case UFBX_PROP_REFERENCE: return "UFBX_PROP_REFERENCE";
		case UFBX_PROP_TYPE_FORCE_32BIT: return "UFBX_PROP_TYPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_prop_flags_str(int value)
{
	switch(value){
		case UFBX_PROP_FLAG_ANIMATABLE: return "UFBX_PROP_FLAG_ANIMATABLE";
		case x1: return "x1";
		case UFBX_PROP_FLAG_USER_DEFINED: return "UFBX_PROP_FLAG_USER_DEFINED";
		case x2: return "x2";
		case UFBX_PROP_FLAG_HIDDEN: return "UFBX_PROP_FLAG_HIDDEN";
		case x4: return "x4";
		case UFBX_PROP_FLAG_LOCK_X: return "UFBX_PROP_FLAG_LOCK_X";
		case x10: return "x10";
		case UFBX_PROP_FLAG_LOCK_Y: return "UFBX_PROP_FLAG_LOCK_Y";
		case x20: return "x20";
		case UFBX_PROP_FLAG_LOCK_Z: return "UFBX_PROP_FLAG_LOCK_Z";
		case x40: return "x40";
		case UFBX_PROP_FLAG_LOCK_W: return "UFBX_PROP_FLAG_LOCK_W";
		case x80: return "x80";
		case UFBX_PROP_FLAG_MUTE_X: return "UFBX_PROP_FLAG_MUTE_X";
		case x100: return "x100";
		case UFBX_PROP_FLAG_MUTE_Y: return "UFBX_PROP_FLAG_MUTE_Y";
		case x200: return "x200";
		case UFBX_PROP_FLAG_MUTE_Z: return "UFBX_PROP_FLAG_MUTE_Z";
		case x400: return "x400";
		case UFBX_PROP_FLAG_MUTE_W: return "UFBX_PROP_FLAG_MUTE_W";
		case x800: return "x800";
		case UFBX_PROP_FLAG_SYNTHETIC: return "UFBX_PROP_FLAG_SYNTHETIC";
		case x1000: return "x1000";
		case UFBX_PROP_FLAG_ANIMATED: return "UFBX_PROP_FLAG_ANIMATED";
		case x2000: return "x2000";
		case UFBX_PROP_FLAG_NOT_FOUND: return "UFBX_PROP_FLAG_NOT_FOUND";
		case x4000: return "x4000";
		case UFBX_PROP_FLAG_CONNECTED: return "UFBX_PROP_FLAG_CONNECTED";
		case x8000: return "x8000";
		case UFBX_PROP_FLAG_NO_VALUE: return "UFBX_PROP_FLAG_NO_VALUE";
		case x10000: return "x10000";
		case UFBX_PROP_FLAG_OVERRIDDEN: return "UFBX_PROP_FLAG_OVERRIDDEN";
		case x20000: return "x20000";
		case UFBX_PROP_FLAG_VALUE_REAL: return "UFBX_PROP_FLAG_VALUE_REAL";
		case x100000: return "x100000";
		case UFBX_PROP_FLAG_VALUE_VEC2: return "UFBX_PROP_FLAG_VALUE_VEC2";
		case x200000: return "x200000";
		case UFBX_PROP_FLAG_VALUE_VEC3: return "UFBX_PROP_FLAG_VALUE_VEC3";
		case x400000: return "x400000";
		case UFBX_PROP_FLAG_VALUE_VEC4: return "UFBX_PROP_FLAG_VALUE_VEC4";
		case x800000: return "x800000";
		case UFBX_PROP_FLAG_VALUE_INT: return "UFBX_PROP_FLAG_VALUE_INT";
		case x1000000: return "x1000000";
		case UFBX_PROP_FLAG_VALUE_STR: return "UFBX_PROP_FLAG_VALUE_STR";
		case x2000000: return "x2000000";
		case UFBX_PROP_FLAG_VALUE_BLOB: return "UFBX_PROP_FLAG_VALUE_BLOB";
		case x4000000: return "x4000000";
		case UFBX_PROP_FLAGS_FORCE_32BIT: return "UFBX_PROP_FLAGS_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_element_type_str(int value)
{
	switch(value){
		case UFBX_ELEMENT_UNKNOWN: return "UFBX_ELEMENT_UNKNOWN";
		case UFBX_ELEMENT_NODE: return "UFBX_ELEMENT_NODE";
		case UFBX_ELEMENT_MESH: return "UFBX_ELEMENT_MESH";
		case UFBX_ELEMENT_LIGHT: return "UFBX_ELEMENT_LIGHT";
		case UFBX_ELEMENT_CAMERA: return "UFBX_ELEMENT_CAMERA";
		case UFBX_ELEMENT_BONE: return "UFBX_ELEMENT_BONE";
		case UFBX_ELEMENT_EMPTY: return "UFBX_ELEMENT_EMPTY";
		case UFBX_ELEMENT_LINE_CURVE: return "UFBX_ELEMENT_LINE_CURVE";
		case UFBX_ELEMENT_NURBS_CURVE: return "UFBX_ELEMENT_NURBS_CURVE";
		case UFBX_ELEMENT_NURBS_SURFACE: return "UFBX_ELEMENT_NURBS_SURFACE";
		case UFBX_ELEMENT_NURBS_TRIM_SURFACE: return "UFBX_ELEMENT_NURBS_TRIM_SURFACE";
		case UFBX_ELEMENT_NURBS_TRIM_BOUNDARY: return "UFBX_ELEMENT_NURBS_TRIM_BOUNDARY";
		case UFBX_ELEMENT_PROCEDURAL_GEOMETRY: return "UFBX_ELEMENT_PROCEDURAL_GEOMETRY";
		case UFBX_ELEMENT_STEREO_CAMERA: return "UFBX_ELEMENT_STEREO_CAMERA";
		case UFBX_ELEMENT_CAMERA_SWITCHER: return "UFBX_ELEMENT_CAMERA_SWITCHER";
		case UFBX_ELEMENT_MARKER: return "UFBX_ELEMENT_MARKER";
		case UFBX_ELEMENT_LOD_GROUP: return "UFBX_ELEMENT_LOD_GROUP";
		case UFBX_ELEMENT_SKIN_DEFORMER: return "UFBX_ELEMENT_SKIN_DEFORMER";
		case UFBX_ELEMENT_SKIN_CLUSTER: return "UFBX_ELEMENT_SKIN_CLUSTER";
		case UFBX_ELEMENT_BLEND_DEFORMER: return "UFBX_ELEMENT_BLEND_DEFORMER";
		case UFBX_ELEMENT_BLEND_CHANNEL: return "UFBX_ELEMENT_BLEND_CHANNEL";
		case UFBX_ELEMENT_BLEND_SHAPE: return "UFBX_ELEMENT_BLEND_SHAPE";
		case UFBX_ELEMENT_CACHE_DEFORMER: return "UFBX_ELEMENT_CACHE_DEFORMER";
		case UFBX_ELEMENT_CACHE_FILE: return "UFBX_ELEMENT_CACHE_FILE";
		case UFBX_ELEMENT_MATERIAL: return "UFBX_ELEMENT_MATERIAL";
		case UFBX_ELEMENT_TEXTURE: return "UFBX_ELEMENT_TEXTURE";
		case UFBX_ELEMENT_VIDEO: return "UFBX_ELEMENT_VIDEO";
		case UFBX_ELEMENT_SHADER: return "UFBX_ELEMENT_SHADER";
		case UFBX_ELEMENT_SHADER_BINDING: return "UFBX_ELEMENT_SHADER_BINDING";
		case UFBX_ELEMENT_ANIM_STACK: return "UFBX_ELEMENT_ANIM_STACK";
		case UFBX_ELEMENT_ANIM_LAYER: return "UFBX_ELEMENT_ANIM_LAYER";
		case UFBX_ELEMENT_ANIM_VALUE: return "UFBX_ELEMENT_ANIM_VALUE";
		case UFBX_ELEMENT_ANIM_CURVE: return "UFBX_ELEMENT_ANIM_CURVE";
		case UFBX_ELEMENT_DISPLAY_LAYER: return "UFBX_ELEMENT_DISPLAY_LAYER";
		case UFBX_ELEMENT_SELECTION_SET: return "UFBX_ELEMENT_SELECTION_SET";
		case UFBX_ELEMENT_SELECTION_NODE: return "UFBX_ELEMENT_SELECTION_NODE";
		case UFBX_ELEMENT_CHARACTER: return "UFBX_ELEMENT_CHARACTER";
		case UFBX_ELEMENT_CONSTRAINT: return "UFBX_ELEMENT_CONSTRAINT";
		case UFBX_ELEMENT_AUDIO_LAYER: return "UFBX_ELEMENT_AUDIO_LAYER";
		case UFBX_ELEMENT_AUDIO_CLIP: return "UFBX_ELEMENT_AUDIO_CLIP";
		case UFBX_ELEMENT_POSE: return "UFBX_ELEMENT_POSE";
		case UFBX_ELEMENT_METADATA_OBJECT: return "UFBX_ELEMENT_METADATA_OBJECT";
		case UFBX_ELEMENT_TYPE_FIRST_ATTRIB: return "UFBX_ELEMENT_TYPE_FIRST_ATTRIB";
		case UFBX_ELEMENT_MESH: return "UFBX_ELEMENT_MESH";
		case UFBX_ELEMENT_TYPE_LAST_ATTRIB: return "UFBX_ELEMENT_TYPE_LAST_ATTRIB";
		case UFBX_ELEMENT_LOD_GROUP: return "UFBX_ELEMENT_LOD_GROUP";
		case UFBX_ELEMENT_TYPE_FORCE_32BIT: return "UFBX_ELEMENT_TYPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_inherit_mode_str(int value)
{
	switch(value){
		case UFBX_INHERIT_MODE_NORMAL: return "UFBX_INHERIT_MODE_NORMAL";
		case UFBX_INHERIT_MODE_IGNORE_PARENT_SCALE: return "UFBX_INHERIT_MODE_IGNORE_PARENT_SCALE";
		case UFBX_INHERIT_MODE_COMPONENTWISE_SCALE: return "UFBX_INHERIT_MODE_COMPONENTWISE_SCALE";
		case UFBX_INHERIT_MODE_FORCE_32BIT: return "UFBX_INHERIT_MODE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_mirror_axis_str(int value)
{
	switch(value){
		case UFBX_MIRROR_AXIS_NONE: return "UFBX_MIRROR_AXIS_NONE";
		case UFBX_MIRROR_AXIS_X: return "UFBX_MIRROR_AXIS_X";
		case UFBX_MIRROR_AXIS_Y: return "UFBX_MIRROR_AXIS_Y";
		case UFBX_MIRROR_AXIS_Z: return "UFBX_MIRROR_AXIS_Z";
		case UFBX_MIRROR_AXIS_FORCE_32BIT: return "UFBX_MIRROR_AXIS_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_subdivision_display_mode_str(int value)
{
	switch(value){
		case UFBX_SUBDIVISION_DISPLAY_DISABLED: return "UFBX_SUBDIVISION_DISPLAY_DISABLED";
		case UFBX_SUBDIVISION_DISPLAY_HULL: return "UFBX_SUBDIVISION_DISPLAY_HULL";
		case UFBX_SUBDIVISION_DISPLAY_HULL_AND_SMOOTH: return "UFBX_SUBDIVISION_DISPLAY_HULL_AND_SMOOTH";
		case UFBX_SUBDIVISION_DISPLAY_SMOOTH: return "UFBX_SUBDIVISION_DISPLAY_SMOOTH";
		case UFBX_SUBDIVISION_DISPLAY_MODE_FORCE_32BIT: return "UFBX_SUBDIVISION_DISPLAY_MODE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_subdivision_boundary_str(int value)
{
	switch(value){
		case UFBX_SUBDIVISION_BOUNDARY_DEFAULT: return "UFBX_SUBDIVISION_BOUNDARY_DEFAULT";
		case UFBX_SUBDIVISION_BOUNDARY_LEGACY: return "UFBX_SUBDIVISION_BOUNDARY_LEGACY";
		case UFBX_SUBDIVISION_BOUNDARY_SHARP_CORNERS: return "UFBX_SUBDIVISION_BOUNDARY_SHARP_CORNERS";
		case UFBX_SUBDIVISION_BOUNDARY_SHARP_NONE: return "UFBX_SUBDIVISION_BOUNDARY_SHARP_NONE";
		case UFBX_SUBDIVISION_BOUNDARY_SHARP_BOUNDARY: return "UFBX_SUBDIVISION_BOUNDARY_SHARP_BOUNDARY";
		case UFBX_SUBDIVISION_BOUNDARY_SHARP_INTERIOR: return "UFBX_SUBDIVISION_BOUNDARY_SHARP_INTERIOR";
		case UFBX_SUBDIVISION_BOUNDARY_FORCE_32BIT: return "UFBX_SUBDIVISION_BOUNDARY_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_light_type_str(int value)
{
	switch(value){
		case UFBX_LIGHT_POINT: return "UFBX_LIGHT_POINT";
		case UFBX_LIGHT_DIRECTIONAL: return "UFBX_LIGHT_DIRECTIONAL";
		case UFBX_LIGHT_SPOT: return "UFBX_LIGHT_SPOT";
		case UFBX_LIGHT_AREA: return "UFBX_LIGHT_AREA";
		case UFBX_LIGHT_VOLUME: return "UFBX_LIGHT_VOLUME";
		case UFBX_LIGHT_TYPE_FORCE_32BIT: return "UFBX_LIGHT_TYPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_light_decay_str(int value)
{
	switch(value){
		case UFBX_LIGHT_DECAY_NONE: return "UFBX_LIGHT_DECAY_NONE";
		case UFBX_LIGHT_DECAY_LINEAR: return "UFBX_LIGHT_DECAY_LINEAR";
		case UFBX_LIGHT_DECAY_QUADRATIC: return "UFBX_LIGHT_DECAY_QUADRATIC";
		case UFBX_LIGHT_DECAY_CUBIC: return "UFBX_LIGHT_DECAY_CUBIC";
		case UFBX_LIGHT_DECAY_FORCE_32BIT: return "UFBX_LIGHT_DECAY_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_light_area_shape_str(int value)
{
	switch(value){
		case UFBX_LIGHT_AREA_SHAPE_RECTANGLE: return "UFBX_LIGHT_AREA_SHAPE_RECTANGLE";
		case UFBX_LIGHT_AREA_SHAPE_SPHERE: return "UFBX_LIGHT_AREA_SHAPE_SPHERE";
		case UFBX_LIGHT_AREA_SHAPE_FORCE_32BIT: return "UFBX_LIGHT_AREA_SHAPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_projection_mode_str(int value)
{
	switch(value){
		case UFBX_PROJECTION_MODE_PERSPECTIVE: return "UFBX_PROJECTION_MODE_PERSPECTIVE";
		case UFBX_PROJECTION_MODE_ORTHOGRAPHIC: return "UFBX_PROJECTION_MODE_ORTHOGRAPHIC";
		case UFBX_PROJECTION_MODE_FORCE_32BIT: return "UFBX_PROJECTION_MODE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_aspect_mode_str(int value)
{
	switch(value){
		case UFBX_ASPECT_MODE_WINDOW_SIZE: return "UFBX_ASPECT_MODE_WINDOW_SIZE";
		case UFBX_ASPECT_MODE_FIXED_RATIO: return "UFBX_ASPECT_MODE_FIXED_RATIO";
		case UFBX_ASPECT_MODE_FIXED_RESOLUTION: return "UFBX_ASPECT_MODE_FIXED_RESOLUTION";
		case UFBX_ASPECT_MODE_FIXED_WIDTH: return "UFBX_ASPECT_MODE_FIXED_WIDTH";
		case UFBX_ASPECT_MODE_FIXED_HEIGHT: return "UFBX_ASPECT_MODE_FIXED_HEIGHT";
		case UFBX_ASPECT_MODE_FORCE_32BIT: return "UFBX_ASPECT_MODE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_aperture_mode_str(int value)
{
	switch(value){
		case UFBX_APERTURE_MODE_HORIZONTAL_AND_VERTICAL: return "UFBX_APERTURE_MODE_HORIZONTAL_AND_VERTICAL";
		case UFBX_APERTURE_MODE_HORIZONTAL: return "UFBX_APERTURE_MODE_HORIZONTAL";
		case UFBX_APERTURE_MODE_VERTICAL: return "UFBX_APERTURE_MODE_VERTICAL";
		case UFBX_APERTURE_MODE_FOCAL_LENGTH: return "UFBX_APERTURE_MODE_FOCAL_LENGTH";
		case UFBX_APERTURE_MODE_FORCE_32BIT: return "UFBX_APERTURE_MODE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_gate_fit_str(int value)
{
	switch(value){
		case UFBX_GATE_FIT_NONE: return "UFBX_GATE_FIT_NONE";
		case UFBX_GATE_FIT_VERTICAL: return "UFBX_GATE_FIT_VERTICAL";
		case UFBX_GATE_FIT_HORIZONTAL: return "UFBX_GATE_FIT_HORIZONTAL";
		case UFBX_GATE_FIT_FILL: return "UFBX_GATE_FIT_FILL";
		case UFBX_GATE_FIT_OVERSCAN: return "UFBX_GATE_FIT_OVERSCAN";
		case UFBX_GATE_FIT_STRETCH: return "UFBX_GATE_FIT_STRETCH";
		case UFBX_GATE_FIT_FORCE_32BIT: return "UFBX_GATE_FIT_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_aperture_format_str(int value)
{
	switch(value){
		case UFBX_APERTURE_FORMAT_CUSTOM: return "UFBX_APERTURE_FORMAT_CUSTOM";
		case UFBX_APERTURE_FORMAT_16MM_THEATRICAL: return "UFBX_APERTURE_FORMAT_16MM_THEATRICAL";
		case UFBX_APERTURE_FORMAT_SUPER_16MM: return "UFBX_APERTURE_FORMAT_SUPER_16MM";
		case UFBX_APERTURE_FORMAT_35MM_ACADEMY: return "UFBX_APERTURE_FORMAT_35MM_ACADEMY";
		case UFBX_APERTURE_FORMAT_35MM_TV_PROJECTION: return "UFBX_APERTURE_FORMAT_35MM_TV_PROJECTION";
		case UFBX_APERTURE_FORMAT_35MM_FULL_APERTURE: return "UFBX_APERTURE_FORMAT_35MM_FULL_APERTURE";
		case UFBX_APERTURE_FORMAT_35MM_185_PROJECTION: return "UFBX_APERTURE_FORMAT_35MM_185_PROJECTION";
		case UFBX_APERTURE_FORMAT_35MM_ANAMORPHIC: return "UFBX_APERTURE_FORMAT_35MM_ANAMORPHIC";
		case UFBX_APERTURE_FORMAT_70MM_PROJECTION: return "UFBX_APERTURE_FORMAT_70MM_PROJECTION";
		case UFBX_APERTURE_FORMAT_VISTAVISION: return "UFBX_APERTURE_FORMAT_VISTAVISION";
		case UFBX_APERTURE_FORMAT_DYNAVISION: return "UFBX_APERTURE_FORMAT_DYNAVISION";
		case UFBX_APERTURE_FORMAT_IMAX: return "UFBX_APERTURE_FORMAT_IMAX";
		case UFBX_APERTURE_FORMAT_FORCE_32BIT: return "UFBX_APERTURE_FORMAT_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_coordinate_axis_str(int value)
{
	switch(value){
		case UFBX_COORDINATE_AXIS_POSITIVE_X: return "UFBX_COORDINATE_AXIS_POSITIVE_X";
		case UFBX_COORDINATE_AXIS_NEGATIVE_X: return "UFBX_COORDINATE_AXIS_NEGATIVE_X";
		case UFBX_COORDINATE_AXIS_POSITIVE_Y: return "UFBX_COORDINATE_AXIS_POSITIVE_Y";
		case UFBX_COORDINATE_AXIS_NEGATIVE_Y: return "UFBX_COORDINATE_AXIS_NEGATIVE_Y";
		case UFBX_COORDINATE_AXIS_POSITIVE_Z: return "UFBX_COORDINATE_AXIS_POSITIVE_Z";
		case UFBX_COORDINATE_AXIS_NEGATIVE_Z: return "UFBX_COORDINATE_AXIS_NEGATIVE_Z";
		case UFBX_COORDINATE_AXIS_UNKNOWN: return "UFBX_COORDINATE_AXIS_UNKNOWN";
		case UFBX_COORDINATE_AXIS_FORCE_32BIT: return "UFBX_COORDINATE_AXIS_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_nurbs_topology_str(int value)
{
	switch(value){
		case UFBX_NURBS_TOPOLOGY_OPEN: return "UFBX_NURBS_TOPOLOGY_OPEN";
		case UFBX_NURBS_TOPOLOGY_PERIODIC: return "UFBX_NURBS_TOPOLOGY_PERIODIC";
		case UFBX_NURBS_TOPOLOGY_CLOSED: return "UFBX_NURBS_TOPOLOGY_CLOSED";
		case UFBX_NURBS_TOPOLOGY_FORCE_32BIT: return "UFBX_NURBS_TOPOLOGY_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_marker_type_str(int value)
{
	switch(value){
		case UFBX_MARKER_UNKNOWN: return "UFBX_MARKER_UNKNOWN";
		case UFBX_MARKER_FK_EFFECTOR: return "UFBX_MARKER_FK_EFFECTOR";
		case UFBX_MARKER_IK_EFFECTOR: return "UFBX_MARKER_IK_EFFECTOR";
		case UFBX_MARKER_TYPE_FORCE_32BIT: return "UFBX_MARKER_TYPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_lod_display_str(int value)
{
	switch(value){
		case UFBX_LOD_DISPLAY_USE_LOD: return "UFBX_LOD_DISPLAY_USE_LOD";
		case UFBX_LOD_DISPLAY_SHOW: return "UFBX_LOD_DISPLAY_SHOW";
		case UFBX_LOD_DISPLAY_HIDE: return "UFBX_LOD_DISPLAY_HIDE";
		case UFBX_LOD_DISPLAY_FORCE_32BIT: return "UFBX_LOD_DISPLAY_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_skinning_method_str(int value)
{
	switch(value){
		case UFBX_SKINNING_METHOD_LINEAR: return "UFBX_SKINNING_METHOD_LINEAR";
		case UFBX_SKINNING_METHOD_RIGID: return "UFBX_SKINNING_METHOD_RIGID";
		case UFBX_SKINNING_METHOD_DUAL_QUATERNION: return "UFBX_SKINNING_METHOD_DUAL_QUATERNION";
		case UFBX_SKINNING_METHOD_BLENDED_DQ_LINEAR: return "UFBX_SKINNING_METHOD_BLENDED_DQ_LINEAR";
		case UFBX_SKINNING_METHOD_FORCE_32BIT: return "UFBX_SKINNING_METHOD_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_cache_file_format_str(int value)
{
	switch(value){
		case UFBX_CACHE_FILE_FORMAT_UNKNOWN: return "UFBX_CACHE_FILE_FORMAT_UNKNOWN";
		case UFBX_CACHE_FILE_FORMAT_PC2: return "UFBX_CACHE_FILE_FORMAT_PC2";
		case UFBX_CACHE_FILE_FORMAT_MC: return "UFBX_CACHE_FILE_FORMAT_MC";
		case UFBX_CACHE_FILE_FORMAT_FORCE_32BIT: return "UFBX_CACHE_FILE_FORMAT_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_cache_data_format_str(int value)
{
	switch(value){
		case UFBX_CACHE_DATA_FORMAT_UNKNOWN: return "UFBX_CACHE_DATA_FORMAT_UNKNOWN";
		case UFBX_CACHE_DATA_FORMAT_REAL_FLOAT: return "UFBX_CACHE_DATA_FORMAT_REAL_FLOAT";
		case UFBX_CACHE_DATA_FORMAT_VEC3_FLOAT: return "UFBX_CACHE_DATA_FORMAT_VEC3_FLOAT";
		case UFBX_CACHE_DATA_FORMAT_REAL_DOUBLE: return "UFBX_CACHE_DATA_FORMAT_REAL_DOUBLE";
		case UFBX_CACHE_DATA_FORMAT_VEC3_DOUBLE: return "UFBX_CACHE_DATA_FORMAT_VEC3_DOUBLE";
		case UFBX_CACHE_DATA_FORMAT_FORCE_32BIT: return "UFBX_CACHE_DATA_FORMAT_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_cache_data_encoding_str(int value)
{
	switch(value){
		case UFBX_CACHE_DATA_ENCODING_UNKNOWN: return "UFBX_CACHE_DATA_ENCODING_UNKNOWN";
		case UFBX_CACHE_DATA_ENCODING_LITTLE_ENDIAN: return "UFBX_CACHE_DATA_ENCODING_LITTLE_ENDIAN";
		case UFBX_CACHE_DATA_ENCODING_BIG_ENDIAN: return "UFBX_CACHE_DATA_ENCODING_BIG_ENDIAN";
		case UFBX_CACHE_DATA_ENCODING_FORCE_32BIT: return "UFBX_CACHE_DATA_ENCODING_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_cache_interpretation_str(int value)
{
	switch(value){
		case UFBX_CACHE_INTERPRETATION_UNKNOWN: return "UFBX_CACHE_INTERPRETATION_UNKNOWN";
		case UFBX_CACHE_INTERPRETATION_POINTS: return "UFBX_CACHE_INTERPRETATION_POINTS";
		case UFBX_CACHE_INTERPRETATION_VERTEX_POSITION: return "UFBX_CACHE_INTERPRETATION_VERTEX_POSITION";
		case UFBX_CACHE_INTERPRETATION_VERTEX_NORMAL: return "UFBX_CACHE_INTERPRETATION_VERTEX_NORMAL";
		case UFBX_CACHE_INTERPRETATION_FORCE_32BIT: return "UFBX_CACHE_INTERPRETATION_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_shader_type_str(int value)
{
	switch(value){
		case UFBX_SHADER_UNKNOWN: return "UFBX_SHADER_UNKNOWN";
		case UFBX_SHADER_FBX_LAMBERT: return "UFBX_SHADER_FBX_LAMBERT";
		case UFBX_SHADER_FBX_PHONG: return "UFBX_SHADER_FBX_PHONG";
		case UFBX_SHADER_OSL_STANDARD_SURFACE: return "UFBX_SHADER_OSL_STANDARD_SURFACE";
		case UFBX_SHADER_ARNOLD_STANDARD_SURFACE: return "UFBX_SHADER_ARNOLD_STANDARD_SURFACE";
		case UFBX_SHADER_3DS_MAX_PHYSICAL_MATERIAL: return "UFBX_SHADER_3DS_MAX_PHYSICAL_MATERIAL";
		case UFBX_SHADER_3DS_MAX_PBR_METAL_ROUGH: return "UFBX_SHADER_3DS_MAX_PBR_METAL_ROUGH";
		case UFBX_SHADER_3DS_MAX_PBR_SPEC_GLOSS: return "UFBX_SHADER_3DS_MAX_PBR_SPEC_GLOSS";
		case UFBX_SHADER_GLTF_MATERIAL: return "UFBX_SHADER_GLTF_MATERIAL";
		case UFBX_SHADER_SHADERFX_GRAPH: return "UFBX_SHADER_SHADERFX_GRAPH";
		case UFBX_SHADER_BLENDER_PHONG: return "UFBX_SHADER_BLENDER_PHONG";
		case UFBX_SHADER_WAVEFRONT_MTL: return "UFBX_SHADER_WAVEFRONT_MTL";
		case UFBX_SHADER_TYPE_FORCE_32BIT: return "UFBX_SHADER_TYPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_material_fbx_map_str(int value)
{
	switch(value){
		case UFBX_MATERIAL_FBX_DIFFUSE_FACTOR: return "UFBX_MATERIAL_FBX_DIFFUSE_FACTOR";
		case UFBX_MATERIAL_FBX_DIFFUSE_COLOR: return "UFBX_MATERIAL_FBX_DIFFUSE_COLOR";
		case UFBX_MATERIAL_FBX_SPECULAR_FACTOR: return "UFBX_MATERIAL_FBX_SPECULAR_FACTOR";
		case UFBX_MATERIAL_FBX_SPECULAR_COLOR: return "UFBX_MATERIAL_FBX_SPECULAR_COLOR";
		case UFBX_MATERIAL_FBX_SPECULAR_EXPONENT: return "UFBX_MATERIAL_FBX_SPECULAR_EXPONENT";
		case UFBX_MATERIAL_FBX_REFLECTION_FACTOR: return "UFBX_MATERIAL_FBX_REFLECTION_FACTOR";
		case UFBX_MATERIAL_FBX_REFLECTION_COLOR: return "UFBX_MATERIAL_FBX_REFLECTION_COLOR";
		case UFBX_MATERIAL_FBX_TRANSPARENCY_FACTOR: return "UFBX_MATERIAL_FBX_TRANSPARENCY_FACTOR";
		case UFBX_MATERIAL_FBX_TRANSPARENCY_COLOR: return "UFBX_MATERIAL_FBX_TRANSPARENCY_COLOR";
		case UFBX_MATERIAL_FBX_EMISSION_FACTOR: return "UFBX_MATERIAL_FBX_EMISSION_FACTOR";
		case UFBX_MATERIAL_FBX_EMISSION_COLOR: return "UFBX_MATERIAL_FBX_EMISSION_COLOR";
		case UFBX_MATERIAL_FBX_AMBIENT_FACTOR: return "UFBX_MATERIAL_FBX_AMBIENT_FACTOR";
		case UFBX_MATERIAL_FBX_AMBIENT_COLOR: return "UFBX_MATERIAL_FBX_AMBIENT_COLOR";
		case UFBX_MATERIAL_FBX_NORMAL_MAP: return "UFBX_MATERIAL_FBX_NORMAL_MAP";
		case UFBX_MATERIAL_FBX_BUMP: return "UFBX_MATERIAL_FBX_BUMP";
		case UFBX_MATERIAL_FBX_BUMP_FACTOR: return "UFBX_MATERIAL_FBX_BUMP_FACTOR";
		case UFBX_MATERIAL_FBX_DISPLACEMENT_FACTOR: return "UFBX_MATERIAL_FBX_DISPLACEMENT_FACTOR";
		case UFBX_MATERIAL_FBX_DISPLACEMENT: return "UFBX_MATERIAL_FBX_DISPLACEMENT";
		case UFBX_MATERIAL_FBX_VECTOR_DISPLACEMENT_FACTOR: return "UFBX_MATERIAL_FBX_VECTOR_DISPLACEMENT_FACTOR";
		case UFBX_MATERIAL_FBX_VECTOR_DISPLACEMENT: return "UFBX_MATERIAL_FBX_VECTOR_DISPLACEMENT";
		case UFBX_MATERIAL_FBX_MAP_FORCE_32BIT: return "UFBX_MATERIAL_FBX_MAP_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_material_pbr_map_str(int value)
{
	switch(value){
		case UFBX_MATERIAL_PBR_BASE_FACTOR: return "UFBX_MATERIAL_PBR_BASE_FACTOR";
		case UFBX_MATERIAL_PBR_BASE_COLOR: return "UFBX_MATERIAL_PBR_BASE_COLOR";
		case UFBX_MATERIAL_PBR_ROUGHNESS: return "UFBX_MATERIAL_PBR_ROUGHNESS";
		case UFBX_MATERIAL_PBR_METALNESS: return "UFBX_MATERIAL_PBR_METALNESS";
		case UFBX_MATERIAL_PBR_DIFFUSE_ROUGHNESS: return "UFBX_MATERIAL_PBR_DIFFUSE_ROUGHNESS";
		case UFBX_MATERIAL_PBR_SPECULAR_FACTOR: return "UFBX_MATERIAL_PBR_SPECULAR_FACTOR";
		case UFBX_MATERIAL_PBR_SPECULAR_COLOR: return "UFBX_MATERIAL_PBR_SPECULAR_COLOR";
		case UFBX_MATERIAL_PBR_SPECULAR_IOR: return "UFBX_MATERIAL_PBR_SPECULAR_IOR";
		case UFBX_MATERIAL_PBR_SPECULAR_ANISOTROPY: return "UFBX_MATERIAL_PBR_SPECULAR_ANISOTROPY";
		case UFBX_MATERIAL_PBR_SPECULAR_ROTATION: return "UFBX_MATERIAL_PBR_SPECULAR_ROTATION";
		case UFBX_MATERIAL_PBR_TRANSMISSION_FACTOR: return "UFBX_MATERIAL_PBR_TRANSMISSION_FACTOR";
		case UFBX_MATERIAL_PBR_TRANSMISSION_COLOR: return "UFBX_MATERIAL_PBR_TRANSMISSION_COLOR";
		case UFBX_MATERIAL_PBR_TRANSMISSION_DEPTH: return "UFBX_MATERIAL_PBR_TRANSMISSION_DEPTH";
		case UFBX_MATERIAL_PBR_TRANSMISSION_SCATTER: return "UFBX_MATERIAL_PBR_TRANSMISSION_SCATTER";
		case UFBX_MATERIAL_PBR_TRANSMISSION_SCATTER_ANISOTROPY: return "UFBX_MATERIAL_PBR_TRANSMISSION_SCATTER_ANISOTROPY";
		case UFBX_MATERIAL_PBR_TRANSMISSION_DISPERSION: return "UFBX_MATERIAL_PBR_TRANSMISSION_DISPERSION";
		case UFBX_MATERIAL_PBR_TRANSMISSION_ROUGHNESS: return "UFBX_MATERIAL_PBR_TRANSMISSION_ROUGHNESS";
		case UFBX_MATERIAL_PBR_TRANSMISSION_EXTRA_ROUGHNESS: return "UFBX_MATERIAL_PBR_TRANSMISSION_EXTRA_ROUGHNESS";
		case UFBX_MATERIAL_PBR_TRANSMISSION_PRIORITY: return "UFBX_MATERIAL_PBR_TRANSMISSION_PRIORITY";
		case UFBX_MATERIAL_PBR_TRANSMISSION_ENABLE_IN_AOV: return "UFBX_MATERIAL_PBR_TRANSMISSION_ENABLE_IN_AOV";
		case UFBX_MATERIAL_PBR_SUBSURFACE_FACTOR: return "UFBX_MATERIAL_PBR_SUBSURFACE_FACTOR";
		case UFBX_MATERIAL_PBR_SUBSURFACE_COLOR: return "UFBX_MATERIAL_PBR_SUBSURFACE_COLOR";
		case UFBX_MATERIAL_PBR_SUBSURFACE_RADIUS: return "UFBX_MATERIAL_PBR_SUBSURFACE_RADIUS";
		case UFBX_MATERIAL_PBR_SUBSURFACE_SCALE: return "UFBX_MATERIAL_PBR_SUBSURFACE_SCALE";
		case UFBX_MATERIAL_PBR_SUBSURFACE_ANISOTROPY: return "UFBX_MATERIAL_PBR_SUBSURFACE_ANISOTROPY";
		case UFBX_MATERIAL_PBR_SUBSURFACE_TINT_COLOR: return "UFBX_MATERIAL_PBR_SUBSURFACE_TINT_COLOR";
		case UFBX_MATERIAL_PBR_SUBSURFACE_TYPE: return "UFBX_MATERIAL_PBR_SUBSURFACE_TYPE";
		case UFBX_MATERIAL_PBR_SHEEN_FACTOR: return "UFBX_MATERIAL_PBR_SHEEN_FACTOR";
		case UFBX_MATERIAL_PBR_SHEEN_COLOR: return "UFBX_MATERIAL_PBR_SHEEN_COLOR";
		case UFBX_MATERIAL_PBR_SHEEN_ROUGHNESS: return "UFBX_MATERIAL_PBR_SHEEN_ROUGHNESS";
		case UFBX_MATERIAL_PBR_COAT_FACTOR: return "UFBX_MATERIAL_PBR_COAT_FACTOR";
		case UFBX_MATERIAL_PBR_COAT_COLOR: return "UFBX_MATERIAL_PBR_COAT_COLOR";
		case UFBX_MATERIAL_PBR_COAT_ROUGHNESS: return "UFBX_MATERIAL_PBR_COAT_ROUGHNESS";
		case UFBX_MATERIAL_PBR_COAT_IOR: return "UFBX_MATERIAL_PBR_COAT_IOR";
		case UFBX_MATERIAL_PBR_COAT_ANISOTROPY: return "UFBX_MATERIAL_PBR_COAT_ANISOTROPY";
		case UFBX_MATERIAL_PBR_COAT_ROTATION: return "UFBX_MATERIAL_PBR_COAT_ROTATION";
		case UFBX_MATERIAL_PBR_COAT_NORMAL: return "UFBX_MATERIAL_PBR_COAT_NORMAL";
		case UFBX_MATERIAL_PBR_COAT_AFFECT_BASE_COLOR: return "UFBX_MATERIAL_PBR_COAT_AFFECT_BASE_COLOR";
		case UFBX_MATERIAL_PBR_COAT_AFFECT_BASE_ROUGHNESS: return "UFBX_MATERIAL_PBR_COAT_AFFECT_BASE_ROUGHNESS";
		case UFBX_MATERIAL_PBR_THIN_FILM_THICKNESS: return "UFBX_MATERIAL_PBR_THIN_FILM_THICKNESS";
		case UFBX_MATERIAL_PBR_THIN_FILM_IOR: return "UFBX_MATERIAL_PBR_THIN_FILM_IOR";
		case UFBX_MATERIAL_PBR_EMISSION_FACTOR: return "UFBX_MATERIAL_PBR_EMISSION_FACTOR";
		case UFBX_MATERIAL_PBR_EMISSION_COLOR: return "UFBX_MATERIAL_PBR_EMISSION_COLOR";
		case UFBX_MATERIAL_PBR_OPACITY: return "UFBX_MATERIAL_PBR_OPACITY";
		case UFBX_MATERIAL_PBR_INDIRECT_DIFFUSE: return "UFBX_MATERIAL_PBR_INDIRECT_DIFFUSE";
		case UFBX_MATERIAL_PBR_INDIRECT_SPECULAR: return "UFBX_MATERIAL_PBR_INDIRECT_SPECULAR";
		case UFBX_MATERIAL_PBR_NORMAL_MAP: return "UFBX_MATERIAL_PBR_NORMAL_MAP";
		case UFBX_MATERIAL_PBR_TANGENT_MAP: return "UFBX_MATERIAL_PBR_TANGENT_MAP";
		case UFBX_MATERIAL_PBR_DISPLACEMENT_MAP: return "UFBX_MATERIAL_PBR_DISPLACEMENT_MAP";
		case UFBX_MATERIAL_PBR_MATTE_FACTOR: return "UFBX_MATERIAL_PBR_MATTE_FACTOR";
		case UFBX_MATERIAL_PBR_MATTE_COLOR: return "UFBX_MATERIAL_PBR_MATTE_COLOR";
		case UFBX_MATERIAL_PBR_AMBIENT_OCCLUSION: return "UFBX_MATERIAL_PBR_AMBIENT_OCCLUSION";
		case UFBX_MATERIAL_PBR_GLOSSINESS: return "UFBX_MATERIAL_PBR_GLOSSINESS";
		case UFBX_MATERIAL_PBR_COAT_GLOSSINESS: return "UFBX_MATERIAL_PBR_COAT_GLOSSINESS";
		case UFBX_MATERIAL_PBR_TRANSMISSION_GLOSSINESS: return "UFBX_MATERIAL_PBR_TRANSMISSION_GLOSSINESS";
		case UFBX_MATERIAL_PBR_MAP_FORCE_32BIT: return "UFBX_MATERIAL_PBR_MAP_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_material_feature_str(int value)
{
	switch(value){
		case UFBX_MATERIAL_FEATURE_PBR: return "UFBX_MATERIAL_FEATURE_PBR";
		case UFBX_MATERIAL_FEATURE_METALNESS: return "UFBX_MATERIAL_FEATURE_METALNESS";
		case UFBX_MATERIAL_FEATURE_DIFFUSE: return "UFBX_MATERIAL_FEATURE_DIFFUSE";
		case UFBX_MATERIAL_FEATURE_SPECULAR: return "UFBX_MATERIAL_FEATURE_SPECULAR";
		case UFBX_MATERIAL_FEATURE_EMISSION: return "UFBX_MATERIAL_FEATURE_EMISSION";
		case UFBX_MATERIAL_FEATURE_TRANSMISSION: return "UFBX_MATERIAL_FEATURE_TRANSMISSION";
		case UFBX_MATERIAL_FEATURE_COAT: return "UFBX_MATERIAL_FEATURE_COAT";
		case UFBX_MATERIAL_FEATURE_SHEEN: return "UFBX_MATERIAL_FEATURE_SHEEN";
		case UFBX_MATERIAL_FEATURE_OPACITY: return "UFBX_MATERIAL_FEATURE_OPACITY";
		case UFBX_MATERIAL_FEATURE_AMBIENT_OCCLUSION: return "UFBX_MATERIAL_FEATURE_AMBIENT_OCCLUSION";
		case UFBX_MATERIAL_FEATURE_MATTE: return "UFBX_MATERIAL_FEATURE_MATTE";
		case UFBX_MATERIAL_FEATURE_UNLIT: return "UFBX_MATERIAL_FEATURE_UNLIT";
		case UFBX_MATERIAL_FEATURE_IOR: return "UFBX_MATERIAL_FEATURE_IOR";
		case UFBX_MATERIAL_FEATURE_DIFFUSE_ROUGHNESS: return "UFBX_MATERIAL_FEATURE_DIFFUSE_ROUGHNESS";
		case UFBX_MATERIAL_FEATURE_TRANSMISSION_ROUGHNESS: return "UFBX_MATERIAL_FEATURE_TRANSMISSION_ROUGHNESS";
		case UFBX_MATERIAL_FEATURE_THIN_WALLED: return "UFBX_MATERIAL_FEATURE_THIN_WALLED";
		case UFBX_MATERIAL_FEATURE_CAUSTICS: return "UFBX_MATERIAL_FEATURE_CAUSTICS";
		case UFBX_MATERIAL_FEATURE_EXIT_TO_BACKGROUND: return "UFBX_MATERIAL_FEATURE_EXIT_TO_BACKGROUND";
		case UFBX_MATERIAL_FEATURE_INTERNAL_REFLECTIONS: return "UFBX_MATERIAL_FEATURE_INTERNAL_REFLECTIONS";
		case UFBX_MATERIAL_FEATURE_DOUBLE_SIDED: return "UFBX_MATERIAL_FEATURE_DOUBLE_SIDED";
		case UFBX_MATERIAL_FEATURE_ROUGHNESS_AS_GLOSSINESS: return "UFBX_MATERIAL_FEATURE_ROUGHNESS_AS_GLOSSINESS";
		case UFBX_MATERIAL_FEATURE_COAT_ROUGHNESS_AS_GLOSSINESS: return "UFBX_MATERIAL_FEATURE_COAT_ROUGHNESS_AS_GLOSSINESS";
		case UFBX_MATERIAL_FEATURE_TRANSMISSION_ROUGHNESS_AS_GLOSSINESS: return "UFBX_MATERIAL_FEATURE_TRANSMISSION_ROUGHNESS_AS_GLOSSINESS";
		case UFBX_MATERIAL_FEATURE_FORCE_32BIT: return "UFBX_MATERIAL_FEATURE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_texture_type_str(int value)
{
	switch(value){
		case UFBX_TEXTURE_FILE: return "UFBX_TEXTURE_FILE";
		case UFBX_TEXTURE_LAYERED: return "UFBX_TEXTURE_LAYERED";
		case UFBX_TEXTURE_PROCEDURAL: return "UFBX_TEXTURE_PROCEDURAL";
		case UFBX_TEXTURE_SHADER: return "UFBX_TEXTURE_SHADER";
		case UFBX_TEXTURE_TYPE_FORCE_32BIT: return "UFBX_TEXTURE_TYPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_blend_mode_str(int value)
{
	switch(value){
		case UFBX_BLEND_TRANSLUCENT: return "UFBX_BLEND_TRANSLUCENT";
		case UFBX_BLEND_ADDITIVE: return "UFBX_BLEND_ADDITIVE";
		case UFBX_BLEND_MULTIPLY: return "UFBX_BLEND_MULTIPLY";
		case UFBX_BLEND_MULTIPLY_2X: return "UFBX_BLEND_MULTIPLY_2X";
		case UFBX_BLEND_OVER: return "UFBX_BLEND_OVER";
		case UFBX_BLEND_REPLACE: return "UFBX_BLEND_REPLACE";
		case UFBX_BLEND_DISSOLVE: return "UFBX_BLEND_DISSOLVE";
		case UFBX_BLEND_DARKEN: return "UFBX_BLEND_DARKEN";
		case UFBX_BLEND_COLOR_BURN: return "UFBX_BLEND_COLOR_BURN";
		case UFBX_BLEND_LINEAR_BURN: return "UFBX_BLEND_LINEAR_BURN";
		case UFBX_BLEND_DARKER_COLOR: return "UFBX_BLEND_DARKER_COLOR";
		case UFBX_BLEND_LIGHTEN: return "UFBX_BLEND_LIGHTEN";
		case UFBX_BLEND_SCREEN: return "UFBX_BLEND_SCREEN";
		case UFBX_BLEND_COLOR_DODGE: return "UFBX_BLEND_COLOR_DODGE";
		case UFBX_BLEND_LINEAR_DODGE: return "UFBX_BLEND_LINEAR_DODGE";
		case UFBX_BLEND_LIGHTER_COLOR: return "UFBX_BLEND_LIGHTER_COLOR";
		case UFBX_BLEND_SOFT_LIGHT: return "UFBX_BLEND_SOFT_LIGHT";
		case UFBX_BLEND_HARD_LIGHT: return "UFBX_BLEND_HARD_LIGHT";
		case UFBX_BLEND_VIVID_LIGHT: return "UFBX_BLEND_VIVID_LIGHT";
		case UFBX_BLEND_LINEAR_LIGHT: return "UFBX_BLEND_LINEAR_LIGHT";
		case UFBX_BLEND_PIN_LIGHT: return "UFBX_BLEND_PIN_LIGHT";
		case UFBX_BLEND_HARD_MIX: return "UFBX_BLEND_HARD_MIX";
		case UFBX_BLEND_DIFFERENCE: return "UFBX_BLEND_DIFFERENCE";
		case UFBX_BLEND_EXCLUSION: return "UFBX_BLEND_EXCLUSION";
		case UFBX_BLEND_SUBTRACT: return "UFBX_BLEND_SUBTRACT";
		case UFBX_BLEND_DIVIDE: return "UFBX_BLEND_DIVIDE";
		case UFBX_BLEND_HUE: return "UFBX_BLEND_HUE";
		case UFBX_BLEND_SATURATION: return "UFBX_BLEND_SATURATION";
		case UFBX_BLEND_COLOR: return "UFBX_BLEND_COLOR";
		case UFBX_BLEND_LUMINOSITY: return "UFBX_BLEND_LUMINOSITY";
		case UFBX_BLEND_OVERLAY: return "UFBX_BLEND_OVERLAY";
		case UFBX_BLEND_MODE_FORCE_32BIT: return "UFBX_BLEND_MODE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_wrap_mode_str(int value)
{
	switch(value){
		case UFBX_WRAP_REPEAT: return "UFBX_WRAP_REPEAT";
		case UFBX_WRAP_CLAMP: return "UFBX_WRAP_CLAMP";
		case UFBX_WRAP_MODE_FORCE_32BIT: return "UFBX_WRAP_MODE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_shader_texture_type_str(int value)
{
	switch(value){
		case UFBX_SHADER_TEXTURE_UNKNOWN: return "UFBX_SHADER_TEXTURE_UNKNOWN";
		case UFBX_SHADER_TEXTURE_SELECT_OUTPUT: return "UFBX_SHADER_TEXTURE_SELECT_OUTPUT";
		case UFBX_SHADER_TEXTURE_OSL: return "UFBX_SHADER_TEXTURE_OSL";
		case UFBX_SHADER_TEXTURE_TYPE_FORCE_32BIT: return "UFBX_SHADER_TEXTURE_TYPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_interpolation_str(int value)
{
	switch(value){
		case UFBX_INTERPOLATION_CONSTANT_PREV: return "UFBX_INTERPOLATION_CONSTANT_PREV";
		case UFBX_INTERPOLATION_CONSTANT_NEXT: return "UFBX_INTERPOLATION_CONSTANT_NEXT";
		case UFBX_INTERPOLATION_LINEAR: return "UFBX_INTERPOLATION_LINEAR";
		case UFBX_INTERPOLATION_CUBIC: return "UFBX_INTERPOLATION_CUBIC";
		case UFBX_INTERPOLATION_FORCE_32BIT: return "UFBX_INTERPOLATION_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_constraint_type_str(int value)
{
	switch(value){
		case UFBX_CONSTRAINT_UNKNOWN: return "UFBX_CONSTRAINT_UNKNOWN";
		case UFBX_CONSTRAINT_AIM: return "UFBX_CONSTRAINT_AIM";
		case UFBX_CONSTRAINT_PARENT: return "UFBX_CONSTRAINT_PARENT";
		case UFBX_CONSTRAINT_POSITION: return "UFBX_CONSTRAINT_POSITION";
		case UFBX_CONSTRAINT_ROTATION: return "UFBX_CONSTRAINT_ROTATION";
		case UFBX_CONSTRAINT_SCALE: return "UFBX_CONSTRAINT_SCALE";
		case UFBX_CONSTRAINT_SINGLE_CHAIN_IK: return "UFBX_CONSTRAINT_SINGLE_CHAIN_IK";
		case UFBX_CONSTRAINT_TYPE_FORCE_32BIT: return "UFBX_CONSTRAINT_TYPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_constraint_aim_up_type_str(int value)
{
	switch(value){
		case UFBX_CONSTRAINT_AIM_UP_SCENE: return "UFBX_CONSTRAINT_AIM_UP_SCENE";
		case UFBX_CONSTRAINT_AIM_UP_TO_NODE: return "UFBX_CONSTRAINT_AIM_UP_TO_NODE";
		case UFBX_CONSTRAINT_AIM_UP_ALIGN_NODE: return "UFBX_CONSTRAINT_AIM_UP_ALIGN_NODE";
		case UFBX_CONSTRAINT_AIM_UP_VECTOR: return "UFBX_CONSTRAINT_AIM_UP_VECTOR";
		case UFBX_CONSTRAINT_AIM_UP_NONE: return "UFBX_CONSTRAINT_AIM_UP_NONE";
		case UFBX_CONSTRAINT_AIM_UP_TYPE_FORCE_32BIT: return "UFBX_CONSTRAINT_AIM_UP_TYPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_constraint_ik_pole_type_str(int value)
{
	switch(value){
		case UFBX_CONSTRAINT_IK_POLE_VECTOR: return "UFBX_CONSTRAINT_IK_POLE_VECTOR";
		case UFBX_CONSTRAINT_IK_POLE_NODE: return "UFBX_CONSTRAINT_IK_POLE_NODE";
		case UFBX_CONSTRAINT_IK_POLE_TYPE_FORCE_32BIT: return "UFBX_CONSTRAINT_IK_POLE_TYPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_exporter_str(int value)
{
	switch(value){
		case UFBX_EXPORTER_UNKNOWN: return "UFBX_EXPORTER_UNKNOWN";
		case UFBX_EXPORTER_FBX_SDK: return "UFBX_EXPORTER_FBX_SDK";
		case UFBX_EXPORTER_BLENDER_BINARY: return "UFBX_EXPORTER_BLENDER_BINARY";
		case UFBX_EXPORTER_BLENDER_ASCII: return "UFBX_EXPORTER_BLENDER_ASCII";
		case UFBX_EXPORTER_MOTION_BUILDER: return "UFBX_EXPORTER_MOTION_BUILDER";
		case UFBX_EXPORTER_FORCE_32BIT: return "UFBX_EXPORTER_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_file_format_str(int value)
{
	switch(value){
		case UFBX_FILE_FORMAT_UNKNOWN: return "UFBX_FILE_FORMAT_UNKNOWN";
		case UFBX_FILE_FORMAT_FBX: return "UFBX_FILE_FORMAT_FBX";
		case UFBX_FILE_FORMAT_OBJ: return "UFBX_FILE_FORMAT_OBJ";
		case UFBX_FILE_FORMAT_MTL: return "UFBX_FILE_FORMAT_MTL";
		case UFBX_FILE_FORMAT_FORCE_32BIT: return "UFBX_FILE_FORMAT_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_warning_type_str(int value)
{
	switch(value){
		case UFBX_WARNING_MISSING_EXTERNAL_FILE: return "UFBX_WARNING_MISSING_EXTERNAL_FILE";
		case UFBX_WARNING_IMPLICIT_MTL: return "UFBX_WARNING_IMPLICIT_MTL";
		case UFBX_WARNING_TRUNCATED_ARRAY: return "UFBX_WARNING_TRUNCATED_ARRAY";
		case UFBX_WARNING_MISSING_GEOMETRY_DATA: return "UFBX_WARNING_MISSING_GEOMETRY_DATA";
		case UFBX_WARNING_DUPLICATE_CONNECTION: return "UFBX_WARNING_DUPLICATE_CONNECTION";
		case UFBX_WARNING_BAD_VERTEX_W_ATTRIBUTE: return "UFBX_WARNING_BAD_VERTEX_W_ATTRIBUTE";
		case UFBX_WARNING_MISSING_POLYGON_MAPPING: return "UFBX_WARNING_MISSING_POLYGON_MAPPING";
		case UFBX_WARNING_INDEX_CLAMPED: return "UFBX_WARNING_INDEX_CLAMPED";
		case UFBX_WARNING_BAD_UNICODE: return "UFBX_WARNING_BAD_UNICODE";
		case UFBX_WARNING_BAD_ELEMENT_CONNECTED_TO_ROOT: return "UFBX_WARNING_BAD_ELEMENT_CONNECTED_TO_ROOT";
		case UFBX_WARNING_DUPLICATE_OBJECT_ID: return "UFBX_WARNING_DUPLICATE_OBJECT_ID";
		case UFBX_WARNING_EMPTY_FACE_REMOVED: return "UFBX_WARNING_EMPTY_FACE_REMOVED";
		case UFBX_WARNING_UNKNOWN_OBJ_DIRECTIVE: return "UFBX_WARNING_UNKNOWN_OBJ_DIRECTIVE";
		case UFBX_WARNING_TYPE_FIRST_DEDUPLICATED: return "UFBX_WARNING_TYPE_FIRST_DEDUPLICATED";
		case UFBX_WARNING_INDEX_CLAMPED: return "UFBX_WARNING_INDEX_CLAMPED";
		case UFBX_WARNING_TYPE_FORCE_32BIT: return "UFBX_WARNING_TYPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_thumbnail_format_str(int value)
{
	switch(value){
		case UFBX_THUMBNAIL_FORMAT_UNKNOWN: return "UFBX_THUMBNAIL_FORMAT_UNKNOWN";
		case UFBX_THUMBNAIL_FORMAT_RGB_24: return "UFBX_THUMBNAIL_FORMAT_RGB_24";
		case UFBX_THUMBNAIL_FORMAT_RGBA_32: return "UFBX_THUMBNAIL_FORMAT_RGBA_32";
		case UFBX_THUMBNAIL_FORMAT_FORCE_32BIT: return "UFBX_THUMBNAIL_FORMAT_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_space_conversion_str(int value)
{
	switch(value){
		case UFBX_SPACE_CONVERSION_TRANSFORM_ROOT: return "UFBX_SPACE_CONVERSION_TRANSFORM_ROOT";
		case UFBX_SPACE_CONVERSION_ADJUST_TRANSFORMS: return "UFBX_SPACE_CONVERSION_ADJUST_TRANSFORMS";
		case UFBX_SPACE_CONVERSION_MODIFY_GEOMETRY: return "UFBX_SPACE_CONVERSION_MODIFY_GEOMETRY";
		case UFBX_SPACE_CONVERSION_FORCE_32BIT: return "UFBX_SPACE_CONVERSION_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_time_mode_str(int value)
{
	switch(value){
		case UFBX_TIME_MODE_DEFAULT: return "UFBX_TIME_MODE_DEFAULT";
		case UFBX_TIME_MODE_120_FPS: return "UFBX_TIME_MODE_120_FPS";
		case UFBX_TIME_MODE_100_FPS: return "UFBX_TIME_MODE_100_FPS";
		case UFBX_TIME_MODE_60_FPS: return "UFBX_TIME_MODE_60_FPS";
		case UFBX_TIME_MODE_50_FPS: return "UFBX_TIME_MODE_50_FPS";
		case UFBX_TIME_MODE_48_FPS: return "UFBX_TIME_MODE_48_FPS";
		case UFBX_TIME_MODE_30_FPS: return "UFBX_TIME_MODE_30_FPS";
		case UFBX_TIME_MODE_30_FPS_DROP: return "UFBX_TIME_MODE_30_FPS_DROP";
		case UFBX_TIME_MODE_NTSC_DROP_FRAME: return "UFBX_TIME_MODE_NTSC_DROP_FRAME";
		case UFBX_TIME_MODE_NTSC_FULL_FRAME: return "UFBX_TIME_MODE_NTSC_FULL_FRAME";
		case UFBX_TIME_MODE_PAL: return "UFBX_TIME_MODE_PAL";
		case UFBX_TIME_MODE_24_FPS: return "UFBX_TIME_MODE_24_FPS";
		case UFBX_TIME_MODE_1000_FPS: return "UFBX_TIME_MODE_1000_FPS";
		case UFBX_TIME_MODE_FILM_FULL_FRAME: return "UFBX_TIME_MODE_FILM_FULL_FRAME";
		case UFBX_TIME_MODE_CUSTOM: return "UFBX_TIME_MODE_CUSTOM";
		case UFBX_TIME_MODE_96_FPS: return "UFBX_TIME_MODE_96_FPS";
		case UFBX_TIME_MODE_72_FPS: return "UFBX_TIME_MODE_72_FPS";
		case UFBX_TIME_MODE_59_94_FPS: return "UFBX_TIME_MODE_59_94_FPS";
		case UFBX_TIME_MODE_FORCE_32BIT: return "UFBX_TIME_MODE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_time_protocol_str(int value)
{
	switch(value){
		case UFBX_TIME_PROTOCOL_SMPTE: return "UFBX_TIME_PROTOCOL_SMPTE";
		case UFBX_TIME_PROTOCOL_FRAME_COUNT: return "UFBX_TIME_PROTOCOL_FRAME_COUNT";
		case UFBX_TIME_PROTOCOL_DEFAULT: return "UFBX_TIME_PROTOCOL_DEFAULT";
		case UFBX_TIME_PROTOCOL_FORCE_32BIT: return "UFBX_TIME_PROTOCOL_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_snap_mode_str(int value)
{
	switch(value){
		case UFBX_SNAP_MODE_NONE: return "UFBX_SNAP_MODE_NONE";
		case UFBX_SNAP_MODE_SNAP: return "UFBX_SNAP_MODE_SNAP";
		case UFBX_SNAP_MODE_PLAY: return "UFBX_SNAP_MODE_PLAY";
		case UFBX_SNAP_MODE_SNAP_AND_PLAY: return "UFBX_SNAP_MODE_SNAP_AND_PLAY";
		case UFBX_SNAP_MODE_FORCE_32BIT: return "UFBX_SNAP_MODE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_topo_flags_str(int value)
{
	switch(value){
		case UFBX_TOPO_NON_MANIFOLD: return "UFBX_TOPO_NON_MANIFOLD";
		case x1: return "x1";
		case UFBX_TOPO_FLAGS_FORCE_32BIT: return "UFBX_TOPO_FLAGS_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_open_file_type_str(int value)
{
	switch(value){
		case UFBX_OPEN_FILE_MAIN_MODEL: return "UFBX_OPEN_FILE_MAIN_MODEL";
		case UFBX_OPEN_FILE_GEOMETRY_CACHE: return "UFBX_OPEN_FILE_GEOMETRY_CACHE";
		case UFBX_OPEN_FILE_OBJ_MTL: return "UFBX_OPEN_FILE_OBJ_MTL";
		case UFBX_OPEN_FILE_TYPE_FORCE_32BIT: return "UFBX_OPEN_FILE_TYPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_error_type_str(int value)
{
	switch(value){
		case UFBX_ERROR_NONE: return "UFBX_ERROR_NONE";
		case UFBX_ERROR_UNKNOWN: return "UFBX_ERROR_UNKNOWN";
		case UFBX_ERROR_FILE_NOT_FOUND: return "UFBX_ERROR_FILE_NOT_FOUND";
		case UFBX_ERROR_EMPTY_FILE: return "UFBX_ERROR_EMPTY_FILE";
		case UFBX_ERROR_EXTERNAL_FILE_NOT_FOUND: return "UFBX_ERROR_EXTERNAL_FILE_NOT_FOUND";
		case UFBX_ERROR_OUT_OF_MEMORY: return "UFBX_ERROR_OUT_OF_MEMORY";
		case UFBX_ERROR_MEMORY_LIMIT: return "UFBX_ERROR_MEMORY_LIMIT";
		case UFBX_ERROR_ALLOCATION_LIMIT: return "UFBX_ERROR_ALLOCATION_LIMIT";
		case UFBX_ERROR_TRUNCATED_FILE: return "UFBX_ERROR_TRUNCATED_FILE";
		case UFBX_ERROR_IO: return "UFBX_ERROR_IO";
		case UFBX_ERROR_CANCELLED: return "UFBX_ERROR_CANCELLED";
		case UFBX_ERROR_UNRECOGNIZED_FILE_FORMAT: return "UFBX_ERROR_UNRECOGNIZED_FILE_FORMAT";
		case UFBX_ERROR_UNINITIALIZED_OPTIONS: return "UFBX_ERROR_UNINITIALIZED_OPTIONS";
		case UFBX_ERROR_ZERO_VERTEX_SIZE: return "UFBX_ERROR_ZERO_VERTEX_SIZE";
		case UFBX_ERROR_TRUNCATED_VERTEX_STREAM: return "UFBX_ERROR_TRUNCATED_VERTEX_STREAM";
		case UFBX_ERROR_INVALID_UTF8: return "UFBX_ERROR_INVALID_UTF8";
		case UFBX_ERROR_FEATURE_DISABLED: return "UFBX_ERROR_FEATURE_DISABLED";
		case UFBX_ERROR_BAD_NURBS: return "UFBX_ERROR_BAD_NURBS";
		case UFBX_ERROR_BAD_INDEX: return "UFBX_ERROR_BAD_INDEX";
		case UFBX_ERROR_NODE_DEPTH_LIMIT: return "UFBX_ERROR_NODE_DEPTH_LIMIT";
		case UFBX_ERROR_THREADED_ASCII_PARSE: return "UFBX_ERROR_THREADED_ASCII_PARSE";
		case UFBX_ERROR_UNSAFE_OPTIONS: return "UFBX_ERROR_UNSAFE_OPTIONS";
		case UFBX_ERROR_DUPLICATE_OVERRIDE: return "UFBX_ERROR_DUPLICATE_OVERRIDE";
		case UFBX_ERROR_TYPE_FORCE_32BIT: return "UFBX_ERROR_TYPE_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_progress_result_str(int value)
{
	switch(value){
		case UFBX_PROGRESS_CONTINUE: return "UFBX_PROGRESS_CONTINUE";
		case x100: return "x100";
		case UFBX_PROGRESS_CANCEL: return "UFBX_PROGRESS_CANCEL";
		case x200: return "x200";
		case UFBX_PROGRESS_RESULT_FORCE_32BIT: return "UFBX_PROGRESS_RESULT_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_index_error_handling_str(int value)
{
	switch(value){
		case UFBX_INDEX_ERROR_HANDLING_CLAMP: return "UFBX_INDEX_ERROR_HANDLING_CLAMP";
		case UFBX_INDEX_ERROR_HANDLING_NO_INDEX: return "UFBX_INDEX_ERROR_HANDLING_NO_INDEX";
		case UFBX_INDEX_ERROR_HANDLING_ABORT_LOADING: return "UFBX_INDEX_ERROR_HANDLING_ABORT_LOADING";
		case UFBX_INDEX_ERROR_HANDLING_UNSAFE_IGNORE: return "UFBX_INDEX_ERROR_HANDLING_UNSAFE_IGNORE";
		case UFBX_INDEX_ERROR_HANDLING_FORCE_32BIT: return "UFBX_INDEX_ERROR_HANDLING_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_unicode_error_handling_str(int value)
{
	switch(value){
		case UFBX_UNICODE_ERROR_HANDLING_REPLACEMENT_CHARACTER: return "UFBX_UNICODE_ERROR_HANDLING_REPLACEMENT_CHARACTER";
		case UFBX_UNICODE_ERROR_HANDLING_UNDERSCORE: return "UFBX_UNICODE_ERROR_HANDLING_UNDERSCORE";
		case UFBX_UNICODE_ERROR_HANDLING_QUESTION_MARK: return "UFBX_UNICODE_ERROR_HANDLING_QUESTION_MARK";
		case UFBX_UNICODE_ERROR_HANDLING_REMOVE: return "UFBX_UNICODE_ERROR_HANDLING_REMOVE";
		case UFBX_UNICODE_ERROR_HANDLING_ABORT_LOADING: return "UFBX_UNICODE_ERROR_HANDLING_ABORT_LOADING";
		case UFBX_UNICODE_ERROR_HANDLING_UNSAFE_IGNORE: return "UFBX_UNICODE_ERROR_HANDLING_UNSAFE_IGNORE";
		case UFBX_UNICODE_ERROR_HANDLING_FORCE_32BIT: return "UFBX_UNICODE_ERROR_HANDLING_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_geometry_transform_handling_str(int value)
{
	switch(value){
		case UFBX_GEOMETRY_TRANSFORM_HANDLING_PRESERVE: return "UFBX_GEOMETRY_TRANSFORM_HANDLING_PRESERVE";
		case UFBX_GEOMETRY_TRANSFORM_HANDLING_HELPER_NODES: return "UFBX_GEOMETRY_TRANSFORM_HANDLING_HELPER_NODES";
		case UFBX_GEOMETRY_TRANSFORM_HANDLING_MODIFY_GEOMETRY: return "UFBX_GEOMETRY_TRANSFORM_HANDLING_MODIFY_GEOMETRY";
		case UFBX_GEOMETRY_TRANSFORM_HANDLING_MODIFY_GEOMETRY_NO_FALLBACK: return "UFBX_GEOMETRY_TRANSFORM_HANDLING_MODIFY_GEOMETRY_NO_FALLBACK";
		case UFBX_GEOMETRY_TRANSFORM_HANDLING_FORCE_32BIT: return "UFBX_GEOMETRY_TRANSFORM_HANDLING_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_inherit_mode_handling_str(int value)
{
	switch(value){
		case UFBX_INHERIT_MODE_HANDLING_PRESERVE: return "UFBX_INHERIT_MODE_HANDLING_PRESERVE";
		case UFBX_INHERIT_MODE_HANDLING_HELPER_NODES: return "UFBX_INHERIT_MODE_HANDLING_HELPER_NODES";
		case UFBX_INHERIT_MODE_HANDLING_COMPENSATE: return "UFBX_INHERIT_MODE_HANDLING_COMPENSATE";
		case UFBX_INHERIT_MODE_HANDLING_COMPENSATE_NO_FALLBACK: return "UFBX_INHERIT_MODE_HANDLING_COMPENSATE_NO_FALLBACK";
		case UFBX_INHERIT_MODE_HANDLING_IGNORE: return "UFBX_INHERIT_MODE_HANDLING_IGNORE";
		case UFBX_INHERIT_MODE_HANDLING_FORCE_32BIT: return "UFBX_INHERIT_MODE_HANDLING_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_pivot_handling_str(int value)
{
	switch(value){
		case UFBX_PIVOT_HANDLING_RETAIN: return "UFBX_PIVOT_HANDLING_RETAIN";
		case UFBX_PIVOT_HANDLING_ADJUST_TO_PIVOT: return "UFBX_PIVOT_HANDLING_ADJUST_TO_PIVOT";
		case UFBX_PIVOT_HANDLING_FORCE_32BIT: return "UFBX_PIVOT_HANDLING_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_baked_key_flags_str(int value)
{
	switch(value){
		case UFBX_BAKED_KEY_STEP_LEFT: return "UFBX_BAKED_KEY_STEP_LEFT";
		case x1: return "x1";
		case UFBX_BAKED_KEY_STEP_RIGHT: return "UFBX_BAKED_KEY_STEP_RIGHT";
		case x2: return "x2";
		case UFBX_BAKED_KEY_STEP_KEY: return "UFBX_BAKED_KEY_STEP_KEY";
		case x4: return "x4";
		case UFBX_BAKED_KEY_KEYFRAME: return "UFBX_BAKED_KEY_KEYFRAME";
		case x8: return "x8";
		case UFBX_BAKED_KEY_REDUCED: return "UFBX_BAKED_KEY_REDUCED";
		case x10: return "x10";
		case UFBX_BAKED_KEY_FORCE_32BIT: return "UFBX_BAKED_KEY_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_bake_step_handling_str(int value)
{
	switch(value){
		case UFBX_BAKE_STEP_HANDLING_DEFAULT: return "UFBX_BAKE_STEP_HANDLING_DEFAULT";
		case UFBX_BAKE_STEP_HANDLING_CUSTOM_DURATION: return "UFBX_BAKE_STEP_HANDLING_CUSTOM_DURATION";
		case UFBX_BAKE_STEP_HANDLING_IDENTICAL_TIME: return "UFBX_BAKE_STEP_HANDLING_IDENTICAL_TIME";
		case UFBX_BAKE_STEP_HANDLING_ADJACENT_DOUBLE: return "UFBX_BAKE_STEP_HANDLING_ADJACENT_DOUBLE";
		case UFBX_BAKE_STEP_HANDLING_IGNORE: return "UFBX_BAKE_STEP_HANDLING_IGNORE";
		case ufbx_bake_step_handling_FORCE_32BIT: return "ufbx_bake_step_handling_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_ufbx_transform_flags_str(int value)
{
	switch(value){
		case UFBX_TRANSFORM_FLAG_IGNORE_SCALE_HELPER: return "UFBX_TRANSFORM_FLAG_IGNORE_SCALE_HELPER";
		case x1: return "x1";
		case UFBX_TRANSFORM_FLAG_IGNORE_COMPONENTWISE_SCALE: return "UFBX_TRANSFORM_FLAG_IGNORE_COMPONENTWISE_SCALE";
		case x2: return "x2";
		case UFBX_TRANSFORM_FLAG_EXPLICIT_INCLUDES: return "UFBX_TRANSFORM_FLAG_EXPLICIT_INCLUDES";
		case x4: return "x4";
		case UFBX_TRANSFORM_FLAG_INCLUDE_TRANSLATION: return "UFBX_TRANSFORM_FLAG_INCLUDE_TRANSLATION";
		case x10: return "x10";
		case UFBX_TRANSFORM_FLAG_INCLUDE_ROTATION: return "UFBX_TRANSFORM_FLAG_INCLUDE_ROTATION";
		case x20: return "x20";
		case UFBX_TRANSFORM_FLAG_INCLUDE_SCALE: return "UFBX_TRANSFORM_FLAG_INCLUDE_SCALE";
		case x40: return "x40";
		case UFBX_TRANSFORM_FLAGS_FORCE_32BIT: return "UFBX_TRANSFORM_FLAGS_FORCE_32BIT";
		case x7fffffff: return "x7fffffff";
	}
}
const char *get_enum_GameButtonType_str(int value)
{
	switch(value){
		case BUTTON_A: return "BUTTON_A";
		case BUTTON_B: return "BUTTON_B";
		case BUTTON_C: return "BUTTON_C";
		case BUTTON_D: return "BUTTON_D";
		case BUTTON_E: return "BUTTON_E";
		case BUTTON_F: return "BUTTON_F";
		case BUTTON_G: return "BUTTON_G";
		case BUTTON_H: return "BUTTON_H";
		case BUTTON_I: return "BUTTON_I";
		case BUTTON_J: return "BUTTON_J";
		case BUTTON_K: return "BUTTON_K";
		case BUTTON_L: return "BUTTON_L";
		case BUTTON_M: return "BUTTON_M";
		case BUTTON_N: return "BUTTON_N";
		case BUTTON_O: return "BUTTON_O";
		case BUTTON_P: return "BUTTON_P";
		case BUTTON_Q: return "BUTTON_Q";
		case BUTTON_R: return "BUTTON_R";
		case BUTTON_S: return "BUTTON_S";
		case BUTTON_T: return "BUTTON_T";
		case BUTTON_U: return "BUTTON_U";
		case BUTTON_V: return "BUTTON_V";
		case BUTTON_W: return "BUTTON_W";
		case BUTTON_X: return "BUTTON_X";
		case BUTTON_Y: return "BUTTON_Y";
		case BUTTON_Z: return "BUTTON_Z";
		case BUTTON_LEFT_CONTROL: return "BUTTON_LEFT_CONTROL";
		case BUTTON_LEFT_SHIFT: return "BUTTON_LEFT_SHIFT";
		case BUTTON_MOUSE_LEFT: return "BUTTON_MOUSE_LEFT";
		case BUTTON_MOUSE_RIGHT: return "BUTTON_MOUSE_RIGHT";
		case BUTTON_SPACE: return "BUTTON_SPACE";
		case BUTTON_F1: return "BUTTON_F1";
		case BUTTON_F2: return "BUTTON_F2";
		case BUTTON_F3: return "BUTTON_F3";
		case BUTTON_F4: return "BUTTON_F4";
		case BUTTON_F5: return "BUTTON_F5";
		case BUTTON_F6: return "BUTTON_F6";
		case BUTTON_F7: return "BUTTON_F7";
		case BUTTON_F8: return "BUTTON_F8";
		case BUTTON_F9: return "BUTTON_F9";
		case BUTTON_F10: return "BUTTON_F10";
		case BUTTON_F11: return "BUTTON_F11";
		case BUTTON_F12: return "BUTTON_F12";
		case BUTTON_COUNT: return "BUTTON_COUNT";
	}
}
const char *get_enum_ShaderType_str(int value)
{
	switch(value){
		case SHADER_TYPE_VERTEX: return "SHADER_TYPE_VERTEX";
		case SHADER_TYPE_FRAGMENT: return "SHADER_TYPE_FRAGMENT";
	}
}
const char *get_enum_InputElementType_str(int value)
{
	switch(value){
		case INPUT_ELEMENT_FLOAT: return "INPUT_ELEMENT_FLOAT";
		case INPUT_ELEMENT_SIGNED_INT: return "INPUT_ELEMENT_SIGNED_INT";
	}
}
const char *get_enum_PrimitiveType_str(int value)
{
	switch(value){
		case PRIMITIVE_TRIANGLES: return "PRIMITIVE_TRIANGLES";
		case PRIMITIVE_LINES: return "PRIMITIVE_LINES";
	}
}
const char *get_enum_VertexBufferUsage_str(int value)
{
	switch(value){
		case VERTEX_BUFFER_IMMUTABLE: return "VERTEX_BUFFER_IMMUTABLE";
		case VERTEX_BUFFER_DYNAMIC: return "VERTEX_BUFFER_DYNAMIC";
	}
}
const char *get_enum_RasterizerFillMode_str(int value)
{
	switch(value){
		case RASTERIZER_FILL_SOLID: return "RASTERIZER_FILL_SOLID";
		case RASTERIZER_FILL_WIREFRAME: return "RASTERIZER_FILL_WIREFRAME";
	}
}
const char *get_enum_RasterizerCullMode_str(int value)
{
	switch(value){
		case RASTERIZER_CULL_NONE: return "RASTERIZER_CULL_NONE";
		case RASTERIZER_CULL_FRONT: return "RASTERIZER_CULL_FRONT";
		case RASTERIZER_CULL_BACK: return "RASTERIZER_CULL_BACK";
	}
}
const char *get_enum_ConstantBufferElementType_str(int value)
{
	switch(value){
		case CONSTANT_BUFFER_ELEMENT_MAT4: return "CONSTANT_BUFFER_ELEMENT_MAT4";
		case CONSTANT_BUFFER_ELEMENT_VEC4: return "CONSTANT_BUFFER_ELEMENT_VEC4";
		case CONSTANT_BUFFER_ELEMENT_VEC3: return "CONSTANT_BUFFER_ELEMENT_VEC3";
		case CONSTANT_BUFFER_ELEMENT_VEC2: return "CONSTANT_BUFFER_ELEMENT_VEC2";
		case CONSTANT_BUFFER_ELEMENT_FLOAT: return "CONSTANT_BUFFER_ELEMENT_FLOAT";
		case CONSTANT_BUFFER_ELEMENT_INT: return "CONSTANT_BUFFER_ELEMENT_INT";
		case CONSTANT_BUFFER_ELEMENT_COUNT: return "CONSTANT_BUFFER_ELEMENT_COUNT";
	}
}
const char *get_enum_EntityType_str(int value)
{
	switch(value){
		case EntityType_Player: return "EntityType_Player";
		case EntityType_Enemy: return "EntityType_Enemy";
		case EntityType_Static: return "EntityType_Static";
	}
}
const char *get_enum_CollisionShapeType_str(int value)
{
	switch(value){
		case COLLISION_SHAPE_TRIANGLES: return "COLLISION_SHAPE_TRIANGLES";
		case COLLISION_SHAPE_ELLIPSOID: return "COLLISION_SHAPE_ELLIPSOID";
	}
}
const char *get_enum_AnimationType_str(int value)
{
	switch(value){
		case ANIMATION_JUMP: return "ANIMATION_JUMP";
		case ANIMATION_SHOOT: return "ANIMATION_SHOOT";
		case ANIMATION_RUN: return "ANIMATION_RUN";
		case ANIMATION_FORWARD_GUN_WALK: return "ANIMATION_FORWARD_GUN_WALK";
		case ANIMATION_BACKWARD_GUN_WALK: return "ANIMATION_BACKWARD_GUN_WALK";
		case ANIMATION_GUN_IDLE: return "ANIMATION_GUN_IDLE";
		case ANIMATION_COUNT: return "ANIMATION_COUNT";
	}
}
const char *get_enum_float_round_style_str(int value)
{
	switch(value){
		case round_indeterminate: return "round_indeterminate";
		case round_toward_zero: return "round_toward_zero";
		case round_to_nearest: return "round_to_nearest";
		case round_toward_infinity: return "round_toward_infinity";
		case round_toward_neg_infinity: return "round_toward_neg_infinity";
	}
}
const char *get_enum_float_denorm_style_str(int value)
{
	switch(value){
		case denorm_indeterminate: return "denorm_indeterminate";
		case denorm_absent: return "denorm_absent";
		case denorm_present: return "denorm_present";
	}
}
const char *get_enum_GizmoMode_str(int value)
{
	switch(value){
		case GIZMO_TRANSLATION: return "GIZMO_TRANSLATION";
		case GIZMO_SCALE: return "GIZMO_SCALE";
		case GIZMO_ROTATION: return "GIZMO_ROTATION";
	}
}
#endif
