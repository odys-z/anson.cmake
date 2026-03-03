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
    # enum
    enum_val  : str = 'enum_val'
    enum_name : str = 'enum_name'

    # class
    base_name  : str = 'base_name'
    class_name : str = 'class_name'

    # template
    templ_entity : str = 't_entity'
    templ_params : str = 't_params'
    templ_type   : str = 't_type'
    templ_comment: str = 't_comment'
    templ_class  : str = 't_class'
    templ_body   : str = 't_body'

    # field
    static      : str = 'is_static'
    storage     : str = 'storage'
    is_field    : str = 'is_field'
    field_decl  : str = 'field_decl'
    field_name  : str = 'field_name'
    field_type  : str = 'field_type'
    field_regist: str = 'field_register'

    # ctor
    func_retype : str = 'func_retype'
    ctor_name   : str = 'ctor_name'
    ctor_params : str = 'ctor_params'
    is_func     : str = "is_func"

q_template_one_comment = f"""(template_declaration
      parameters: (template_parameter_list (comment) @{qv.templ_comment}) @{qv.templ_params}
      [
        (class_specifier
            name: (_) @{qv.templ_class}
            body: (_) @{qv.templ_body}
        )
        ;; (function_definition declarator: (_) @{qv.templ_class})
        ;; (declaration)
        ;; (type_definition)
      ] @{qv.templ_entity}
    )
"""

q_template_multi_types = f"""(template_declaration
      parameters: (template_parameter_list (
                    (type_parameter_declaration) @{qv.templ_type}) (comment) @{qv.templ_comment}) @{qv.templ_params}
      [
        (class_specifier
            name: (_) @{qv.templ_class}
            body: (_) @{qv.templ_body}
        )
        ;; (function_definition declarator: (_) @{qv.templ_class})
        ;; (declaration)
        ;; (type_definition)
      ] @{qv.templ_entity}
    )
"""

q_class_fields: str = f"""(class_specifier
        name: (type_identifier) @{qv.class_name}
        (base_class_clause (type_identifier) @{qv.base_name})?
        body: (field_declaration_list 
            [
                ;; 1.2 Capture Data Members specifically
                (field_declaration 
                    (storage_class_specifier)* @{qv.storage}
                    type: (type_specifier) @{qv.field_type}
                    declarator: [
                        ((field_identifier) @{qv.field_name}) @{qv.field_regist}
                        (reference_declarator (field_identifier) @{qv.field_name}) @{qv.field_regist}
                        (pointer_declarator declarator: (field_identifier) @{qv.field_name}) @{qv.field_regist}
                        (pointer_declarator declarator: (pointer_declarator declarator: (field_identifier) @{qv.field_name})) @{qv.field_regist}
                        ;; (pointer_declarator declarator: (pointer_declarator declarator: (pointer_declarator declarator: field_identifier) @field_name))
                    ] @{qv.field_decl}) @{qv.is_field}

                ;; 1.2 Capture Generic Data Members specifically
                (field_declaration 
                    (storage_class_specifier)* @{qv.storage}
                    type: (template_type
                        arguments: (template_argument_list
                          [
                            (type_identifier) @{qv.field_name}
                            (primitive_type) @type_param_id
                          ]
                        ) @{qv.field_type}
                    declarator: [
                        ((field_identifier) @{qv.field_name}) @{qv.field_regist}
                        (reference_declarator (field_identifier) @{qv.field_name}) @{qv.field_regist}
                        (pointer_declarator declarator: (field_identifier) @{qv.field_name}) @{qv.field_regist}
                        (pointer_declarator declarator: (pointer_declarator declarator: (field_identifier) @{qv.field_name})) @{qv.field_regist}
                    ] @{qv.field_decl}) ) @{qv.is_field}

                ;; 2. Capture Function Prototypes (without bodies)
                (field_declaration
                    (storage_class_specifier)* @{qv.storage}
                    type: (_) @{qv.func_retype}
                    declarator: (function_declarator
                        declarator: [
                            (field_identifier) @{qv.ctor_name}
                            (identifier) @{qv.ctor_name}
                        ]
                        parameters: (parameter_list) @{qv.ctor_params})) @{qv.is_func}
                        
                ;; 2.1 Capture Abstract Function Prototypes (without bodies) (Note 1)
 
                ;; 3. Capture Function Definitions (with bodies)
                (function_definition
                    type: (_)? @{qv.func_retype}
                    declarator: (function_declarator
                        declarator: [
                            (field_identifier) @{qv.ctor_name}
                            (identifier) @{qv.ctor_name}
                        ]
                        parameters: (parameter_list) @{qv.ctor_params})) @{qv.is_func}
            ]
        ))
"""

q_templ_class: str = f"""(template_declaration
      parameters: (template_parameter_list (
                    (type_parameter_declaration) @{qv.templ_type}) (comment) @{qv.templ_comment}) @{qv.templ_params}
      [{q_class_fields}
      ] @{qv.templ_entity}
    )
"""

template_class_dag: str = f"""
    (enum_specifier 
        name: (type_identifier) @{qv.enum_name}
        body: (enumerator_list (enumerator name: (identifier) @{qv.enum_val})))
    ;; {{q_template_one_comment}}
    
    {q_templ_class}
    
    {q_class_fields}
"""

field_id_isfunc: str = f"""
    (enum_specifier 
        name: (type_identifier) @{qv.enum_name}
        body: (enumerator_list (enumerator name: (identifier) @{qv.enum_val})))
    ;; {{q_template_one_comment}}
    {q_template_multi_types}

    (class_specifier
        name: (type_identifier) @{qv.class_name}
        (base_class_clause (type_identifier) @{qv.base_name})?
        body: (field_declaration_list 
            [
                ;; 1. Capture Data Members specifically
                (field_declaration 
                    (storage_class_specifier)* @{qv.storage}
                    type: (type_specifier) @{qv.field_type}
                    declarator: [
                        ((field_identifier) @{qv.field_name}) @{qv.field_regist}
                        (reference_declarator (field_identifier) @{qv.field_name}) @{qv.field_regist}
                        (pointer_declarator declarator: (field_identifier) @{qv.field_name}) @{qv.field_regist}
                        (pointer_declarator declarator: (pointer_declarator declarator: (field_identifier) @{qv.field_name})) @{qv.field_regist}
                        ;; (pointer_declarator declarator: (pointer_declarator declarator: (pointer_declarator declarator: field_identifier) @field_name)))
                    ] @{qv.field_decl}) @{qv.is_field}

                ;; 2. Capture Function Prototypes (without bodies)
                (field_declaration
                    (storage_class_specifier)* @{qv.storage}
                    type: (_) @{qv.func_retype}
                    declarator: (function_declarator
                        declarator: [
                            (field_identifier) @{qv.ctor_name}
                            (identifier) @{qv.ctor_name}
                        ]
                        parameters: (parameter_list) @{qv.ctor_params})) @{qv.is_func}
                        
                ;; 2.1 Capture Abstract Function Prototypes (without bodies) (Note 1)
 
                ;; 3. Capture Function Definitions (with bodies)
                (function_definition
                    type: (_)? @{qv.func_retype}
                    declarator: (function_declarator
                        declarator: [
                            (field_identifier) @{qv.ctor_name}
                            (identifier) @{qv.ctor_name}
                        ]
                        parameters: (parameter_list) @{qv.ctor_params})) @{qv.is_func}
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