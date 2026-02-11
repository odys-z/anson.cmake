field_decl_list: str = """
        (enum_specifier 
            name: (type_identifier) @enum_name
            body: (enumerator_list (enumerator name: (identifier) @enum_val)))

        (class_specifier
            name: (type_identifier) @class_name
            body: (field_declaration_list (field_declaration) @field))
"""

virtual_funcs: str = """
    (class_specifier
        name: (type_identifier) @class_name
        (base_class_clause (type_identifier) @base_name)?
        body: (field_declaration_list 
            [
                (field_declaration
                    declarator: (abstract_function_declarator)) @is_virtual
            ]
        ))
"""
'''
Not working
'''

class qv():
    # class
    class_name  : str = 'class_name'

    # template
    templ_entity: str = 't_entity'
    templ_name  : str = 't_name'
    templ_params: str = 't_params'
    templ_body  : str = 't_body'

field_id_isfunc: str = f"""
    (enum_specifier 
        name: (type_identifier) @enum_name
        body: (enumerator_list (enumerator name: (identifier) @enum_val)))
        
    (template_declaration
      parameters: (template_parameter_list) @{qv.templ_params}
      [
        (class_specifier
            name: (_) @{qv.templ_name}
            body: (_) @{qv.templ_body}
        )
        ;; (function_definition declarator: (_) @{qv.templ_name})
        ;; (declaration)
        ;; (type_definition)
      ] @{qv.templ_entity}
    )

    (class_specifier
        name: (type_identifier) @{qv.class_name}
        (base_class_clause (type_identifier) @base_name)?
        body: (field_declaration_list 
            [
                ;; 1. Capture Data Members specifically
                (field_declaration 
                    (storage_class_specifier)* @storage
                    type: (type_specifier) @field_type
                    declarator: [
                        ((field_identifier) @field_name) @field_register
                        (reference_declarator (field_identifier) @field_name) @field_register
                        (pointer_declarator declarator: (field_identifier) @field_name) @field_register
                        (pointer_declarator declarator: (pointer_declarator declarator: (field_identifier) @field_name)) @field_register
                        ;; (pointer_declarator declarator: (pointer_declarator declarator: (pointer_declarator declarator: field_identifier) @field_name)))
                    ] @field_decl) @is_field

                ;; 2. Capture Function Prototypes (without bodies)
                (field_declaration
                    (storage_class_specifier)* @storage
                    type: (_) @func_ret_type
                    declarator: (function_declarator
                        declarator: [
                            (field_identifier) @ctor_name
                            (identifier) @ctor_name
                        ]
                        parameters: (parameter_list) @ctor_params)) @is_func
                        
                ;; 2.1 Capture Abstract Function Prototypes (without bodies) (Nort 1)
 
                ;; 3. Capture Function Definitions (with bodies)
                (function_definition
                    type: (_)? @func_ret_type
                    declarator: (function_declarator
                        declarator: [
                            (field_identifier) @ctor_name
                            (identifier) @ctor_name
                        ]
                        parameters: (parameter_list) @ctor_params)) @is_func
            ]
        ))
"""
'''
* Note 1.
    
    About virtual: Constructors cannot be virtual. All virtual functions currently cannot be queried.
'''

field_id_isfunc_1: str = """
    (enum_specifier 
        name: (type_identifier) @enum_name
        body: (enumerator_list (enumerator name: (identifier) @enum_val)))

    (class_specifier
        name: (type_identifier) @class_name
        (base_class_clause (type_identifier) @base_name)?
        body: (field_declaration_list 
            [
                ;; 1. Capture Data Members specifically
                (field_declaration 
                    (storage_class_specifier)* @storage
                    type: (type_specifier) @field_type
                    declarator: [
                        ((field_identifier) @field_name) @field_register
                        (reference_declarator (field_identifier) @field_name) @field_register
                        (pointer_declarator declarator: (field_identifier) @field_name) @field_register
                        (pointer_declarator declarator: (pointer_declarator declarator: (field_identifier) @field_name)) @field_register
                        ;; (pointer_declarator declarator: (pointer_declarator declarator: (pointer_declarator declarator: field_identifier) @field_name)))
                    ] @field_decl) @is_field

                ;; 2. Capture Function Prototypes (without bodies)
                (field_declaration
                    (storage_class_specifier)* @storage
                    type: (_) @func_ret_type
                    declarator: (function_declarator
                        declarator: [
                            (field_identifier) @ctor_name
                            (identifier) @ctor_name
                        ]
                        parameters: (parameter_list) @ctor_params)) @is_func

                ;; 3. Capture Function Definitions (with bodies)
                (function_definition
                    type: (_)? @func_ret_type
                    declarator: (function_declarator
                        declarator: [
                            (field_identifier) @ctor_name
                            (identifier) @ctor_name
                        ]
                        parameters: (parameter_list) @ctor_params)) @is_func
            ]
        ))
"""

fieldeclpointer_funcdecl_fdretdecl : str = """
    (enum_specifier 
        name: (type_identifier) @enum_name
        body: (enumerator_list (enumerator name: (identifier) @enum_val)))

    (class_specifier
        name: (type_identifier) @class_name
        (base_class_clause (type_identifier) @base_name)?
        body: (field_declaration_list 
            [
                ;; 1. Variable fields (must not be a function_declarator)
                (field_declaration 
                    (storage_class_specifier)* @storage
                    type: (type_identifier) @field_type
                    declarator: [
                        (identifier) 
                        (pointer_declarator) 
                        (reference_declarator)
                    ] @field_decl)

                ;; 2. Function declarations (prototypes)
                (field_declaration
                    type: (_) @func_ret_type
                    declarator: (function_declarator
                        declarator: (identifier) @ctor_name
                        parameters: (parameter_list) @ctor_params))

                ;; 3. Function definitions (with bodies)
                (function_definition
                    type: (_)? @func_ret_type
                    declarator: (function_declarator
                        declarator: (identifier) @ctor_name
                        parameters: (parameter_list) @ctor_params))
            ]
        ))
"""

fieldecl_funcdecl_funcdef : str = """
    (enum_specifier 
        name: (type_identifier) @enum_name
        body: (enumerator_list (enumerator name: (identifier) @enum_val)))

    (class_specifier
        name: (type_identifier) @class_name
        (base_class_clause (type_identifier) @base_name)?
        body: (field_declaration_list 
            [
                ;; Capture member variables and their storage specifiers
                (field_declaration 
                    (storage_class_specifier)* @storage
                    type: (_) @field_type
                    ;; Capture the whole declarator tree
                    declarator: (_) @field_decl)
                
                ;; Capture constructors
                (declaration
                    declarator: (function_declarator
                        declarator: (identifier) @ctor_name
                        parameters: (parameter_list) @ctor_params))
                        
                ;;
                (function_definition
                    declarator: (function_declarator
                        declarator: (identifier) @ctor_name
                        parameters: (parameter_list) @ctor_params))
            ]
        ))
"""