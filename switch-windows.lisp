(defun other-window-backward (&optional n)
  "Move cursor to Nth previous window"
  (interactive "P")
  (other-window (- (prefix-numeric-value n))))

(define-key global-map "\C-x\C-n" 'other-window)
(define-key global-map "\C-x\C-p" 'other-window-backward)

