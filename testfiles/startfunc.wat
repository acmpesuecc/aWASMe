(module
  (func $start
    ;; simple side-effect example
    nop
  )

  (start $start)

  (func (export "getNumber") (result i32)
    i32.const 42
  )
)