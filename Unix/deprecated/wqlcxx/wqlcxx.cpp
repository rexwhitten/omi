/*
**==============================================================================
**
** Copyright (c) Microsoft Corporation. All rights reserved. See file LICENSE
** for license information.
**
**==============================================================================
*/

#include "wqlcxx.h"
#include "wql/wql.h"

using namespace wqlcxx;

extern "C"
{
    static int _lookup(
        const MI_Char* name,
        const ZChar* /* embeddedClass */,
        const ZChar* /* embeddedType */,
        ::WQL_Symbol* symbol,
        Batch* batch,
        void* data)
    {
        WQLValueSource* source = reinterpret_cast<WQLValueSource*>(data);
        WQLType type;
        WQLValue value;

        if (!source->LookupValue(name, type, value))
            return -1;

        switch (type)
        {
            case WQLTYPE_BOOLEAN:
            {
                symbol->type = WQL_TYPE_BOOLEAN;
                symbol->value.boolean = value.booleanValue;
                return 0;
            }
            case WQLTYPE_INTEGER:
            {
                symbol->type = WQL_TYPE_INTEGER;
                symbol->value.integer = value.integerValue;
                return 0;
            }
            case WQLTYPE_REAL:
            {
                symbol->type = WQL_TYPE_REAL;
                symbol->value.real = value.realValue;
                return 0;
            }
            case WQLTYPE_STRING:
            {
                symbol->type = WQL_TYPE_STRING;
                symbol->value.string =
                    Batch_Strdup(batch, value.stringValue.c_str());
                return 0;
            }
            default:
                break;
        };

        return -1;
    }
}

WQLCXX_BEGIN

WQLValueSource::~WQLValueSource()
{
}

WQL::WQL()
{
}

WQL::~WQL()
{
    ::WQL* wql = (::WQL*)_rep;
    WQL_Delete(wql);
}

WQL* WQL::Parse(const char* text)
{
    ::WQL* wql = WQL_Parse(text, NULL, WQL_DIALECT_WQL);

    if (!wql)
        return 0;

    WQL* self = new WQL;
    self->_rep = wql;

    return self;
}

void WQL::Destroy(WQL* wql)
{
    delete wql;
}

const char* WQL::GetClassName() const
{
    const ::WQL* wql = (const ::WQL*)_rep;
    return wql->className;
}

size_t WQL::GetPropertyCount() const
{
    const ::WQL* wql = (const ::WQL*)_rep;
    return wql->nproperties;
}

const char* WQL::GetPropertyName(size_t i) const
{
    const ::WQL* wql = (const ::WQL*)_rep;
    return wql->properties[i];
}

bool WQL::Evaluate(const WQLValueSource* source) const
{
    const ::WQL* wql = (const ::WQL*)_rep;

    if (WQL_Eval(wql, _lookup, (void*)source) == 0)
        return true;

    return false;
}

void WQL::Dump() const
{
    const ::WQL* wql = (const ::WQL*)_rep;
    WQL_Dump(wql, 0);
}

WQLCXX_END
