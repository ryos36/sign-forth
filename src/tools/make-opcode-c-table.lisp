(defun chars-to-str-or-sym (first-char chars)
  (let ((str (coerce (nreverse chars) 'string)))
    (if (char= first-char #\')
      (with-input-from-string (stream str)
	(read stream))
      str)))


(defun segment-line-reader (stream &optional (ch \#Space) )
  (let ((white-num 0) chars lst first-char top-char)
    (do ((curr (read-char stream t)
	       (read-char stream t)))
      ((or (null curr) (char= #\newline curr))
       (if chars
	 (push (chars-to-str-or-sym first-char chars) lst)))

      (if (null top-char)
	(setf top-char curr))

      (if (and (char= curr #\Space) (null chars))
      	(incf white-num))

      (if (and (null first-char) (not (char= curr #\Space)))
	(setf first-char curr))

      (if (char= curr ch)
	  (when chars
	    (push (chars-to-str-or-sym first-char chars) lst)
	    (setf chars nil first-char nil))
	  (push curr chars)))

    (if (and top-char (char= top-char #\;)) 'nil
      (cons white-num
	    (nreverse lst)))))


; xxxx-xxxx-xxxx1 ; count x
(defun str-to-count-x (str) 
  (let (flag)
    (count-if (lambda (x) 
		(if flag nil 
		  (cond ((eq x #\x) t) 
			((eq x #\-) nil)
			(t (progn (setf flag t)  nil)))))
	      str)))

; xxxx-xxxx-xx01 ; '(2 b01) = '(2 1)
(defun str-to-op-num (str) 
  (let (flag (bit 0) (num 0))
    (map 'vector (lambda (x) 
		   (setf num (* num 2))
		(if flag nil 
		  (cond ((eq x #\x) nil) 
			((eq x #\-) nil)
			((eq x #\0) (incf bit))
			((eq x #\1) (incf bit) (incf num))
			(t (progn (setf flag t)  nil)))))
	      str)
    (list bit num)))



(defun =p-with-mask (num mask-list)
  (let* ((mask-value (- (expt 2 (car mask-list)) 1))
	 (value (cadr mask-list))
	 (masked-num (logand mask-value num)))
    ;(format t "~a ~a~%" masked-num value)
    (= masked-num value)))


(defun make-table (stream num opcode-list)
  (do ((i 0 (1+ i)) (hit nil nil))
    ((= i num))
    (do ((elm-list opcode-list (cdr elm-list)))
      ((or hit (null elm-list)))

      (let* ((elm (car elm-list))
	     (mask-list (cadr elm))
	     (opcode (caddr elm)))
	(when (=p-with-mask i mask-list)
	  (setf hit t)
	  (format stream "~a ~s~%" i opcode))))
    (if (null hit)
      (format stream "~a \"\"~%" i))))

#|
(multiple-value-bind (a b)
  (str-to-op-num (symbol-name(car table-list)))
	(print a)
	(print b)
	(print #\newline)
	)

;(with-input-from-string (stream "I'm not amused\\n")
;  (segment-line-reader stream #\Space))

|#
(let (opcode-list)
  (with-open-file (stream "opcode.txt")
    (do ((char (peek-char nil stream nil :eof) (peek-char nil stream nil :eof)))
      ((eq char :eof))
      (let ((line (segment-line-reader stream #\Space)))
	(if line
	  (push (list (str-to-count-x (cadr line))
		      (str-to-op-num (cadr line))
		      (caddr line)
		      ) opcode-list)))))

  (make-table t 256 opcode-list))
