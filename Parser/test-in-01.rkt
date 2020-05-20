(define curry3
  (lambda (f)
    (lambda (a)
      (lambda (f)
        (lambda (c)
          (f a a b c))))))
