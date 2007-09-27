;; Location for privately maintained packages.
(add-to-list 'load-path "~/.emacs.d/site-lisp")

;; This is what we want!
;; http://info.borland.com/techpubs/jbuilder/jbuilder9/introjb/key_cua.html
;; These are the bindings also used in early Borland C++ versions, like 3.0
;; the last known "good" release.
; function keys
;; 1help, 2save, 3load, 4goto, 5zoom, 6next, 7split, 8exit!, 9make, 10other, 11hilit, 12undo
;;       C2save as,                                         C9Build
;;                                                          S9debug
;; emacs: must use command name to allow interactive input

(global-set-key [f1] 'info)
(global-set-key [C-f1] 'woman)        ;; Context help

(global-set-key [f2] 'save-buffer)    ;; Save current
(global-set-key [C-f2] 'write-buffer) ;; Save as

(global-set-key [f3] 'find-file)
(global-set-key [f4] 'goto-line)
(global-set-key [F5] 'gdb-toggle-bp-this-line)
(global-set-key [C-F5] 'gud-watch)
(global-set-key [S-f5] 'insert-date)

(global-set-key [f6] 'other-window)   ;;
(global-set-key [C-f6] 'other-frame)
(global-set-key [f7] 'previous-error)
;;(global-set-key [f7] 'gdb-next)   ;; Only set in GDB mode?
(global-set-key [S-f7] 'gdb-next)   ;;

(global-set-key [f8] 'next-error)
(global-set-key [S-f8] 'gdb-step)   ;; Only set in GDB mode?

(global-set-key [f9] 'compile)
(global-set-key [C-f9] 'compile)
(global-set-key [S-f9] 'gdb)

;;
(global-set-key [C-prior] 'beginning-of-buffer)
(global-set-key [C-next] 'end-of-buffer)

;;; Key binding for switching to next and previous buffer
(global-set-key '[C-tab] 'bs-cycle-next)
(global-set-key '[C-S-tab] 'bs-cycle-previous)

(custom-set-variables
  ;; custom-set-variables was added by Custom.
  ;; If you edit it by hand, you could mess it up, so be careful.
  ;; Your init file should contain only one such instance.
  ;; If there is more than one, they won't work right.
 '(column-number-mode t)
 '(cua-enable-cua-keys nil)
 '(desktop-save-mode t)
 '(diff-default-read-only t)
 '(diff-mode-hook (quote (diff-make-unified)))
 '(diff-switches "-u")
 '(dired-ls-F-marks-symlinks t)
 '(display-time-24hr-format t)
 '(display-time-day-and-date nil)
 '(display-time-default-load-average nil)
 '(display-time-mail-file (quote none))
 '(global-auto-revert-mode t)
 '(indicate-buffer-boundaries (quote ((top . left) (bottom . right))))
 '(indicate-empty-lines t)
 '(inhibit-splash-screen t)
 '(save-place t nil (saveplace))
 '(scroll-bar-mode (quote right))
 '(show-paren-mode t)
 '(transient-mark-mode t)
 '(vc-consult-headers nil)
 '(vc-dired-recurse nil)
 '(vc-dired-terse-display nil)
 '(vc-display-status nil)
 '(version-control (quote never))
 '(wdired-enable nil)
 '(which-function-mode t))
(custom-set-faces
  ;; custom-set-faces was added by Custom.
  ;; If you edit it by hand, you could mess it up, so be careful.
  ;; Your init file should contain only one such instance.
  ;; If there is more than one, they won't work right.
 )
