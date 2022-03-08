#ifndef FLAGHELPER
#define FLAGHELPER

#define MustDelete(vCn) ((vCn->Active.State & ControllerStateFlags::MUST_DELETE) == ControllerStateFlags::MUST_DELETE)
#define CheckAtGet(vCn) ((vCn->Active.State & ControllerStateFlags::CHECKED_GET) == ControllerStateFlags::CHECKED_GET)
#define IsReadOnly(vCn) ((vCn->Active.State & ControllerStateFlags::IS_READONLY) == ControllerStateFlags::IS_READONLY)
#define IsInverted(vCn) ((vCn->Active.State & ControllerStateFlags::IS_INVERTED) == ControllerStateFlags::IS_INVERTED)

#define SpinDirect(vC) (IsInverted(vC) * -2 + 1)
#define ChangeSpin(vcn) ( ( ( ControllerStateFlags( IsInverted(vcn) \
 ? ( vcn->Active &= ControllerStateFlags::IS_INVERTED ) \
 : ( vcn->Active |= ControllerStateFlags::IS_INVERTED ) \
) & ControllerStateFlags::IS_INVERTED ) == ControllerStateFlags::IS_INVERTED ) * -2 + 1 )

#else

#undef MustDelete
#undef CheckAtGet
#undef IsReadOnly
#undef IsInverted
#undef SpinDirect
#undef ChangeSpin
#undef FLAGHELPER

#endif