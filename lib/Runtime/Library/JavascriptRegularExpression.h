//-------------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
//-------------------------------------------------------------------------------------------------------
#pragma once

namespace Js
{
    class JavascriptRegExp : public DynamicObject
    {
        friend class JavascriptRegularExpressionType;
        friend class RegexHelper;

    private:
        static PropertyId const specialPropertyIdsAll[];
        static PropertyId const specialPropertyIdsWithoutUnicode[];
        static const uint defaultSpecialPropertyIdsCount = 6;

        UnifiedRegex::RegexPattern* pattern;

        // The pattern used by String.prototype.split could be different than the normal pattern. Even
        // when the sticky flag is present in the normal pattern, split() should look for the pattern
        // in the whole input string as opposed to just looking for it at the beginning.
        //
        // Initialization of this pattern is deferred until split() is called, or it's copied from another
        // RegExp object.
        UnifiedRegex::RegexPattern* splitPattern;

        Var lastIndexVar;  // null => must build lastIndexVar from current lastIndex

    public:

        // Three states for lastIndex value:
        //  1. lastIndexVar has been updated, we must calculate lastIndex from it when we next need it
        static const CharCount NotCachedValue = (CharCount)-2;
    private:
        //  2. ToNumber(lastIndexVar) yields +inf or -inf or an integer not in range [0, MaxCharCount]
        static const CharCount InvalidValue = CharCountFlag;
        //  3. ToNumber(lastIndexVar) yields NaN, +0, -0 or an integer in range [0, MaxCharCount]
        CharCount lastIndexOrFlag;

        static JavascriptRegExp * GetJavascriptRegExp(Arguments& args, PCWSTR varName, ScriptContext* scriptContext);
        static JavascriptRegExp * ToRegExp(Var var, PCWSTR varName, ScriptContext* scriptContext);
        static RecyclableObject * GetThisObject(Arguments& args, PCWSTR varName, ScriptContext* scriptContext);
        static JavascriptString * GetFirstStringArg(Arguments& args, ScriptContext* scriptContext);

        bool GetPropertyBuiltIns(PropertyId propertyId, Var* value, BOOL* result);
        bool SetPropertyBuiltIns(PropertyId propertyId, Var value, PropertyOperationFlags flags, BOOL* result);
        bool GetSetterBuiltIns(PropertyId propertyId, PropertyValueInfo* info, DescriptorFlags* result);
        inline PropertyId const * GetSpecialPropertyIdsInlined() const;

        Var GetOptions();

        inline void SetPattern(UnifiedRegex::RegexPattern* pattern);
        inline void SetSplitPattern(UnifiedRegex::RegexPattern* splitPattern);

        static void SetLastIndexProperty(Var instance, Var lastIndex, ScriptContext* scriptContext);

        static bool HasObservableExec(RecyclableObject* instance, ScriptContext* scriptContext);
        static bool HasObservableGlobalFlag(RecyclableObject* instance, ScriptContext* scriptContext);
        static bool HasObservableStickyFlag(RecyclableObject* instance, ScriptContext* scriptContext);
        static bool HasObservableUnicodeFlag(RecyclableObject* instance, ScriptContext* scriptContext);
        static bool HasObservableFlag(RecyclableObject* instance, PropertyId propertyId, JavascriptFunction* builtinGetter, ScriptContext* scriptContext);
        static bool HasObservableLastIndex(RecyclableObject* instance, ScriptContext* scriptContext);
        static BOOL GetNonProxyDescriptor(RecyclableObject* instance, PropertyId propertyId, ScriptContext *scriptContext, PropertyDescriptor* descriptor);
        static BOOL GetNonProxyDescriptor(RecyclableObject* instance, PropertyId propertyId, ScriptContext *scriptContext, PropertyDescriptor* descriptor, RecyclableObject** descriptorInstance);

        static Var CallExec(RecyclableObject* thisObj, JavascriptString* string, PCWSTR varName, ScriptContext* scriptContext);
        void RecompilePatternForExecIfNeeded(ScriptContext* scriptContext);
        UnifiedRegex::RegexFlags SetRegexFlag(PropertyId propertyId, UnifiedRegex::RegexFlags flags, UnifiedRegex::RegexFlags flag, ScriptContext* scriptContext);

        // For boxing stack instance
        JavascriptRegExp(JavascriptRegExp * instance);

        DEFINE_MARSHAL_OBJECT_TO_SCRIPT_CONTEXT(JavascriptRegExp);
    protected:
        DEFINE_VTABLE_CTOR(JavascriptRegExp, DynamicObject);
    public:
        JavascriptRegExp(UnifiedRegex::RegexPattern* pattern, DynamicType* type);
        JavascriptRegExp(DynamicType * type);

        static uint GetOffsetOfPattern() { return offsetof(JavascriptRegExp, pattern); }
        static uint GetOffsetOfSplitPattern() { return offsetof(JavascriptRegExp, splitPattern); }
        static uint GetOffsetOfLastIndexVar() { return offsetof(JavascriptRegExp, lastIndexVar); }
        static uint GetOffsetOfLastIndexOrFlag() { return offsetof(JavascriptRegExp, lastIndexOrFlag); }

        inline UnifiedRegex::RegexPattern* GetPattern() const { return pattern; }
        inline UnifiedRegex::RegexPattern* GetSplitPattern() const { return splitPattern; }

        InternalString GetSource() const;
        UnifiedRegex::RegexFlags GetFlags() const;

        void CacheLastIndex();

        inline CharCountOrFlag GetLastIndex()
        {
            if (lastIndexOrFlag == NotCachedValue)
            {
                CacheLastIndex();
            }
            return lastIndexOrFlag;
        }

        inline void SetLastIndex(CharCount lastIndex)
        {
            Assert(lastIndex <= MaxCharCount);
            lastIndexVar = 0;
            this->lastIndexOrFlag = lastIndex;
        }

        static bool Is(Var aValue);
        static JavascriptRegExp* FromVar(Var aValue);

        static JavascriptRegExp* CreateRegEx(const wchar_t* pSource, CharCount sourceLen,
            UnifiedRegex::RegexFlags flags, ScriptContext *scriptContext);
        static JavascriptRegExp* CreateRegEx(Var aValue, Var options, ScriptContext *scriptContext);
        static JavascriptRegExp* CreateRegExNoCoerce(Var aValue, Var options, ScriptContext *scriptContext);
        static UnifiedRegex::RegexPattern* CreatePattern(Var aValue, Var options, ScriptContext *scriptContext);
        static Var OP_NewRegEx(Var aCompiledRegex, ScriptContext* scriptContext);

        JavascriptString *ToString(bool sourceOnly = false, bool useFlagsProperty = false);

        class EntryInfo
        {
        public:
            static FunctionInfo NewInstance;
            static FunctionInfo Exec;
            static FunctionInfo Test;
            static FunctionInfo ToString;
            static FunctionInfo SymbolMatch;
            static FunctionInfo SymbolSearch;
            static FunctionInfo GetterSymbolSpecies;
            static FunctionInfo GetterGlobal;
            static FunctionInfo GetterFlags;
            static FunctionInfo GetterIgnoreCase;
            static FunctionInfo GetterMultiline;
            static FunctionInfo GetterOptions;
            static FunctionInfo GetterSource;
            static FunctionInfo GetterSticky;
            static FunctionInfo GetterUnicode;
            // v5.8 only
            static FunctionInfo Compile;
        };

        static Var NewInstance(RecyclableObject* function, CallInfo callInfo, ...);
        static Var EntryExec(RecyclableObject* function, CallInfo callInfo, ...);
        static Var EntryTest(RecyclableObject* function, CallInfo callInfo, ...);
        static Var EntryToString(RecyclableObject* function, CallInfo callInfo, ...);
        static Var EntrySymbolMatch(RecyclableObject* function, CallInfo callInfo, ...);
        static Var EntrySymbolSearch(RecyclableObject* function, CallInfo callInfo, ...);
        static Var EntryGetterSymbolSpecies(RecyclableObject* function, CallInfo callInfo, ...);
        static Var EntryGetterFlags(RecyclableObject* function, CallInfo callInfo, ...);
        static void AppendFlagForFlagsProperty(StringBuilder<ArenaAllocator>* builder, RecyclableObject* thisObj, PropertyId propertyId, wchar_t flag, ScriptContext* scriptContext);
        static Var EntryGetterGlobal(RecyclableObject* function, CallInfo callInfo, ...);
        static Var EntryGetterIgnoreCase(RecyclableObject* function, CallInfo callInfo, ...);
        static Var EntryGetterMultiline(RecyclableObject* function, CallInfo callInfo, ...);
        static Var EntryGetterOptions(RecyclableObject* function, CallInfo callInfo, ...);
        static Var EntryGetterSource(RecyclableObject* function, CallInfo callInfo, ...);
        static Var EntryGetterSticky(RecyclableObject* function, CallInfo callInfo, ...);
        static Var EntryGetterUnicode(RecyclableObject* function, CallInfo callInfo, ...);
        // v5.8 only
        static Var EntryCompile(RecyclableObject* function, CallInfo callInfo, ...);

        virtual bool HasReadOnlyPropertiesInvisibleToTypeHandler() override { return true; }

        virtual BOOL HasProperty(PropertyId propertyId) override;
        virtual BOOL GetProperty(Var originalInstance, PropertyId propertyId, Var* value, PropertyValueInfo* info, ScriptContext* requestContext) override;
        virtual BOOL GetProperty(Var originalInstance, JavascriptString* propertyNameString, Var* value, PropertyValueInfo* info, ScriptContext* requestContext) override;
        virtual BOOL GetPropertyReference(Var originalInstance, PropertyId propertyId, Var* value, PropertyValueInfo* info, ScriptContext* requestContext) override;
        virtual BOOL SetProperty(PropertyId propertyId, Var value, PropertyOperationFlags flags, PropertyValueInfo* info) override;
        virtual BOOL SetProperty(JavascriptString* propertyNameString, Var value, PropertyOperationFlags flags, PropertyValueInfo* info) override;
        virtual BOOL InitProperty(PropertyId propertyId, Var value, PropertyOperationFlags flags = PropertyOperation_None, PropertyValueInfo* info = NULL) override;
        virtual BOOL DeleteProperty(PropertyId propertyId, PropertyOperationFlags flags) override;
        virtual DescriptorFlags GetSetter(PropertyId propertyId, Var* setterValue, PropertyValueInfo* info, ScriptContext* requestContext) override;
        virtual DescriptorFlags GetSetter(JavascriptString* propertyNameString, Var* setterValue, PropertyValueInfo* info, ScriptContext* requestContext) override;
        virtual BOOL GetDiagValueString(StringBuilder<ArenaAllocator>* stringBuilder, ScriptContext* requestContext) override;
        virtual BOOL GetDiagTypeString(StringBuilder<ArenaAllocator>* stringBuilder, ScriptContext* requestContext) override;
        virtual BOOL IsEnumerable(PropertyId propertyId) override;
        virtual BOOL IsConfigurable(PropertyId propertyId) override;
        virtual BOOL IsWritable(PropertyId propertyId) override;
        virtual BOOL GetSpecialPropertyName(uint32 index, Var *propertyName, ScriptContext * requestContext) override;
        virtual uint GetSpecialPropertyCount() const override;
        virtual PropertyId const * GetSpecialPropertyIds() const override;

        static Js::JavascriptRegExp * BoxStackInstance(Js::JavascriptRegExp * instance);
    };

} // namespace Js
