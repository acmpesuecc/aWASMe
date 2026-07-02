(module
  (func $calculator (param $a f32) (param $b f32) (param $c i32) (result f32)

    local.get $c
    i32.const 0
    i32.eq
    (if (result f32)
      (then
        local.get $a
        local.get $b
        f32.add
      )
      (else
        local.get $c
        i32.const 1
        i32.eq
        (if (result f32)
          (then
            local.get $a
            local.get $b
            f32.sub
          )
          (else
            local.get $c
            i32.const 2
            i32.eq
            (if (result f32)
              (then
                local.get $a
                local.get $b
                f32.mul
              )
              (else
                f32.const -1
              )
            )
          )
        )
      )
    )
  )

  (export "calculator" (func $calculator))
)