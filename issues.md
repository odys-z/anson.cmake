# Introducing Try Catches.

Try-catch are recommended by Gemini in c++ 20.

## Error Types

```
    For File I/O Errors: std::ios_base::failure
    For Network Failures: std::system_error

    logic_error: violating an API contract
    runtime_error: A specific category of runtime errors.
    exception: The base class for all standard errors.
```

## Stack Unwinding

Walking Up the Stack (Cleaning) -> Landing in the Catch Block

The Golden Rule: RAII (Resource Acquisition Is Initialization)

1. The Stack Cleans Itself: Every local object in the bypassed frames has its
   destructor called in reverse order of creation.

2. Heap Memory Needs Protection: Raw pointers (new/delete) will leak. Always
   use smart pointers (std::unique_ptr, std::shared_ptr) or standard containers (std::string, std::vector), which are stack objects that manage heap memory safely via RAII.

## Entt can be safe:

entt::meta_any my_object = entt::resolve<MyClass>().construct(arg1, arg2);

dengour:

Scenario A: Constructing into a Raw Pointer

entt::meta_any instance = entt::resolve<MyClass*>().construct(); 
// If this instance holds a raw pointer and an exception is thrown,
// the pointer variable is destroyed, but the heap memory it points to leaks!

Scenario B: Custom Allocators with Exceptions in the Constructor

```
    [ Higher Stack Frame: catch(...) ]
        ▲
        │  (Exception bubbles up, triggering stack unwinding)
        │
    [ Lower Stack Frame: calling EnTT ]
    │  
    ├── local variable: entt::meta_any  ◄── [Destructor called automatically here!]
    └── ── ── ── ── ── ── ─┬─ ─ ─ ─ ─ ─ 
                        │ (manages)
                        ▼
                [ Heap Memory Area ]
                └── Actual instance of MyClass ◄── [Safely deleted by meta_any]
```