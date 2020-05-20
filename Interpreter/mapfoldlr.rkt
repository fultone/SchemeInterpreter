(define my-map
  (lambda (function lst)
    (if (null? lst)
        (quote ())
        (cons (function (car lst))
              (my-map function (cdr lst))))))

(define addOne
  (lambda (x)
    (+ x 1)))

; take first item, apply function to initializer and that first item, and that becomes your new initializer
(define my-foldl
  (lambda (function init lst)
    (if (null? lst)
        init
        (my-foldl function (function (car lst) init) (cdr lst)))))

; take the first item and apply function to the first item and the result of foldr on everything else
(define my-foldr
  (lambda (function init lst)
    (if (null? lst)
        init
        (function (car lst) (my-foldr function init (cdr lst))))))

(define cons2
    (lambda (arg1 arg2)
        (cons arg1 arg2)))

(define minus
    (lambda (arg1 arg2)
        (- arg1 arg2)))

;(my-map addOne (quote (1 2 3 4)))
(my-foldl cons (quote ()) (quote (1 2 3 4)))
;(my-foldr cons (quote ()) (quote (1 2 3 4)))
;(my-foldl - 0 (quote (1 2 3 4)))
;(my-foldr - 0 (quote (1 2 3 4)))
