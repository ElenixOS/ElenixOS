/**
 * @file sni_type_ids.h
 * @brief Generated SNI type identifiers and range classification.
 **************************************************************************
 * Do not edit manually. Regenerate with generate_sni.py.
 **************************************************************************
 */

#ifndef SNI_TYPE_IDS_H
#define SNI_TYPE_IDS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/

/* Public macros ----------------------------------------------*/
#define SNI_TYPE_IS_NUMBER(type) ((type) >= __SNI_TYPE_NUMBER_START && (type) <= __SNI_TYPE_NUMBER_END)
#define SNI_TYPE_IS_HANDLE(type) ((type) >= __SNI_HANDLE_START && (type) <= __SNI_HANDLE_END)
#define SNI_TYPE_IS_VALUE(type) ((type) >= __SNI_VALUE_START && (type) <= __SNI_VALUE_END)
#define SNI_TYPE_IS_TREE_NODE(type) ((type) == SNI_H_LV_OBJ)
#define SNI_TYPE_IS_MANAGED_RESOURCE(type) ((type) > __SNI_HANDLE_RESOURCE_START && (type) < __SNI_HANDLE_RESOURCE_END)
#define SNI_TYPE_IS_TREE_DEPENDENT(type) \
    ((type) > __SNI_TREE_DEPENDENT_RESOURCE_START && (type) < __SNI_TREE_DEPENDENT_RESOURCE_END)
#define SNI_TYPE_IS_HYBRID(type) ((type) > __SNI_HYBRID_RESOURCE_START && (type) < __SNI_HYBRID_RESOURCE_END)
#define SNI_TYPE_IS_PURE_MANAGED(type) \
    ((type) > __SNI_PURE_MANAGED_RESOURCE_START && (type) < __SNI_PURE_MANAGED_RESOURCE_END)
#define SNI_HANDLE_COUNT (__SNI_HANDLE_END - __SNI_HANDLE_START - 1)
#define SNI_MANAGED_RESOURCE_COUNT (__SNI_HANDLE_RESOURCE_END - __SNI_HANDLE_RESOURCE_START - 1)

/* Public typedefs --------------------------------------------*/
typedef enum
{
    __SNI_TYPE_START = 0,
    SNI_T_UNKNOWN = 0,

    __SNI_TYPE_NUMBER_START,
    SNI_T_UINT8,
    SNI_T_INT8,
    SNI_T_UINT16,
    SNI_T_INT16,
    SNI_T_UINT32,
    SNI_T_INT32,
    SNI_T_DOUBLE,
    SNI_T_FLOAT,
    __SNI_TYPE_NUMBER_END,

    SNI_T_BOOL,
    SNI_T_STRING,
    SNI_T_PTR,

    __SNI_HANDLE_START,
    SNI_H_LV_OBJ,
    __SNI_HANDLE_RESOURCE_START,

    __SNI_TREE_DEPENDENT_RESOURCE_START,
    SNI_H_LV_CHART_CURSOR,
    SNI_H_LV_CHART_SERIES,
    SNI_H_LV_EVENT_CB,
    SNI_H_LV_EVENT_DSC,
    SNI_H_EOS_ANALOG_HAND,
    __SNI_TREE_DEPENDENT_RESOURCE_END,

    __SNI_HYBRID_RESOURCE_START,
    SNI_H_EOS_ACTIVITY,
    SNI_H_EOS_VIEW,
    __SNI_HYBRID_RESOURCE_END,

    __SNI_PURE_MANAGED_RESOURCE_START,
    SNI_H_LV_TIMER,
    SNI_H_LV_STYLE,
    SNI_H_LV_ANIM,
    SNI_H_LV_FONT,
    SNI_H_LV_GROUP,
    SNI_H_LV_LAYER,
    SNI_H_LV_OBSERVER,
    SNI_H_LV_DRAW_BUF,
    SNI_H_LV_SUBJECT,
    SNI_H_LV_COLOR_FILTER_DSC,
    __SNI_PURE_MANAGED_RESOURCE_END,
    SNI_H_INT32,
    SNI_H_LV_DISPLAY,
    SNI_H_LV_DRAW_ARC_DSC,
    SNI_H_LV_DRAW_IMAGE_DSC,
    SNI_H_LV_DRAW_LABEL_DSC,
    SNI_H_LV_DRAW_LINE_DSC,
    SNI_H_LV_DRAW_RECT_DSC,
    SNI_H_LV_EVENT,
    SNI_H_LV_GRAD_DSC,
    SNI_H_LV_IMAGE_DSC,
    SNI_H_LV_OBJ_CLASS,
    SNI_H_LV_OBJ_TREE_WALK_CB,
    SNI_H_LV_STYLE_TRANSITION_DSC,
    SNI_H_LV_STYLE_VALUE,

    __SNI_HANDLE_RESOURCE_END,
    __SNI_HANDLE_END,

    __SNI_VALUE_START,
    SNI_V_LV_AREA,
    SNI_V_LV_CALENDAR_DATE,
    SNI_V_LV_COLOR,
    SNI_V_LV_COLOR32,
    SNI_V_LV_GRAD_STOP,
    SNI_V_LV_POINT,
    __SNI_VALUE_END,

    __SNI_TYPE_MAX
} sni_type_t;

#ifdef __cplusplus
}
#endif

#endif /* SNI_TYPE_IDS_H */
