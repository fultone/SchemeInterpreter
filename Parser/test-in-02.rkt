; Input: a lazy-list and an integer
; Returns: the n-th value in the lazy-list (counting from 1)
;          or false if n > length(lazy-list)
(define nth
  (lambda (lazy-list n)
    (cond
      [(or (not (integer? n)) (< n 1)) #f]
      [(equal? lazy-list #f) #f]
      [(equal? n 1) (car lazy-list)]
      [else (nth ((cdr lazy-list)) (- n 1))])))
