;; Turns a traditional Scheme list into a lazy version of that list
(define make-lazy
  (lambda (lst)
    (if (null? lst)
        #f ; if the list is empty, return #f
        (cons (car lst)
              (lambda () (make-lazy (cdr lst)))))))