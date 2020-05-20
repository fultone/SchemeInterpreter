(require rackunit)
(check-equal? (cons-each (quote 1) (quote ((a b) (c) () (d e)))) (quote ((1 a b) (1 c) (1) (1 d e))))