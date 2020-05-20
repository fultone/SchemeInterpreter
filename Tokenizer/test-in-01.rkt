(define keep-first-n
  (lambda (n L)
    (cond ((< n 0) "no negative numbers allowed") ;; test comment
          ((> n (length L)) "n is too big")
          ((= n 0) ()) #f
          (else (append (list (car L))
                        (keep-first-n (- n 1) (cdr L)))))));other test comment