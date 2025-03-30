#include "definition.h"
#include "runtime.h"
#include "wrapper/lua_wrapper.h"

#include <c-sdk/lookup_table.h>

lua::Definitions::IDefinition::IDefinition(lua::State* state) :
    m_State(state),
    m_LookupTable(lua::Runtime::GetInstance()->GetLookupTable())
{

}