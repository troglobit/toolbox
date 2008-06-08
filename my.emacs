;; Location for privately maintained packages.
(add-to-list 'load-path "~/.emacs.d/site-lisp")

;;; Initialize external packages, from Debian.
(let ((startup-file "/usr/share/emacs/site-lisp/debian-startup.el"))
  (if (and (or (not (fboundp 'debian-startup))
               (not (boundp  'debian-emacs-flavor)))
           (file-readable-p startup-file))
      (progn
        (load-file startup-file)
        ;; Create a copy of /etc/emacs into /etc/emacs22 and *only* load that dir.
        (setq debian-emacs-flavor 'emacs22)
        (setq flavor debian-emacs-flavor)
        (debian-run-directories (concat "/etc/" (symbol-name debian-emacs-flavor) "/site-start.d")))))

;;; Autoload Stefan Reichoer's cool psvn
(require 'psvn)

;;; Autoload SCO UNIX Cscope
(require 'xcscope)

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
(global-set-key '[C-S-iso-lefttab] 'bs-cycle-previous)

;; Johans hack with dabbrev
(global-set-key [backtab]    'dabbrev-expand)

(defun linux-c-mode ()
  "C mode with adjusted defaults for use with the Linux kernel."
  (interactive)
  (c-mode)
  (c-set-style "K&R")
  (setq tab-width 8)
  (setq indent-tabs-mode t)
  (setq c-basic-offset 8))
  
(setq auto-mode-alist (cons '("/usr/src/linux.*/.*\\.[ch]$" . linux-c-mode)
                            auto-mode-alist))

;; Add color to a shell running in emacs 'M-x shell'
(autoload 'ansi-color-for-comint-mode-on "ansi-color" nil t)
(add-hook 'shell-mode-hook 'ansi-color-for-comint-mode-on)

;; ============================
;; Mouse Settings
;; ============================

;; mouse button one drags the scroll bar
(global-set-key [vertical-scroll-bar down-mouse-1] 'scroll-bar-drag)

;; setup scroll mouse settings
(defun up-slightly () (interactive) (scroll-up 5))
(defun down-slightly () (interactive) (scroll-down 5))
(global-set-key [mouse-4] 'down-slightly)
(global-set-key [mouse-5] 'up-slightly)

(defun up-one () (interactive) (scroll-up 1))
(defun down-one () (interactive) (scroll-down 1))
(global-set-key [S-mouse-4] 'down-one)
(global-set-key [S-mouse-5] 'up-one)

(defun up-a-lot () (interactive) (scroll-up))
(defun down-a-lot () (interactive) (scroll-down))
(global-set-key [C-mouse-4] 'down-a-lot)
(global-set-key [C-mouse-5] 'up-a-lot)

;; ============================
;; Display
;; ============================

;; disable startup message
(setq inhibit-startup-message t)

;; ===========================
;; Behaviour
;; ===========================

;; Make all yes-or-no questions as y-or-n
(fset 'yes-or-no-p 'y-or-n-p)

;; Pgup/dn will return exactly to the starting point.
(setq scroll-preserve-screen-position 1)

;; don't automatically add new lines when scrolling down at
;; the bottom of a buffer
(setq next-line-add-newlines nil)

;; scroll just one line when hitting the bottom of the window
(setq scroll-step 1)
(setq scroll-conservatively 1)

;; format the title-bar to always include the buffer name
(setq frame-title-format "emacs - %b")

;; show a menu only when running within X (save real estate when
;; in console)
(menu-bar-mode (if window-system 1 -1))

;; turn off the toolbar
(if (>= emacs-major-version 21)
    (tool-bar-mode -1))

;; turn on word wrapping in text mode
(add-hook 'text-mode-hook 'turn-on-auto-fill)

;; replace highlighted text with what I type rather than just
;; inserting at a point
(delete-selection-mode t)

;; resize the mini-buffer when necessary
(setq resize-minibuffer-mode t)

;; highlight during searching
(setq query-replace-highlight t)

;; highlight incremental search
(setq search-highlight t)

;; ===========================
;; Custom Functions
;; ===========================

;; print an ascii table
(defun ascii-table ()
  (interactive)
  (switch-to-buffer "*ASCII*")
  (erase-buffer)
  (insert (format "ASCII characters up to number %d.\n" 254))
  (let ((i 0))
    (while (< i 254)
      (setq i (+ i 1))
      (insert (format "%4d %c\n" i i))))
  (beginning-of-buffer))

;; indent the entire buffer
(defun c-indent-buffer ()
  "Indent entire buffer of C source code."
  (interactive)
  (save-excursion
    (goto-char (point-min))
    (while (< (point) (point-max))
      (c-indent-command)
      (end-of-line)
      (forward-char 1))))

;; insert functions
(global-unset-key "\C-t")
(global-set-key "\C-t\C-h" 'insert-function-header)

(defun insert-function-header () (interactive)
  (insert "/**\n")
  (insert " * function - Short description.\n")
  (insert " * @param: Description\n")
  (insert " * \n")
  (insert " * Returns: \n")
  (insert " * \n")
  (insert " */\n"))

(global-set-key "\C-t\C-b" 'insert-file-header)

(defun insert-file-header () (interactive)
  (insert "/* \\\\/ Westermo OnTime - <FILE DESCRIPTION>\n")
  (insert " *\n")
  (insert " * Copyright (C) 2008  Westermo OnTime AS\n")
  (insert " *\n")
  (insert " * Author: Joachim Nilsson <joachim.nilsson@westermo.se>\n")
  (insert " *\n")
  (insert " * Description:\n")
  (insert " *\n")
  (insert " */\n"))

(global-set-key "\C-t\C-i" 'insert-include-body)

(defun insert-include-body () (interactive)
  (insert "/* \\\\/ Westermo OnTime - <FILE DESCRIPTION>\n")
  (insert " *\n")
  (insert " * Copyright (C) 2008  Westermo OnTime AS\n")
  (insert " *\n")
  (insert " * Author: Joachim Nilsson <joachim.nilsson@westermo.se>\n")
  (insert " *\n")
  (insert " * Description:\n")
  (insert " *\n")
  (insert " */\n")
  (insert "\n")
  (insert "#ifndef __FILE_H__\n")
  (insert "#define __FILE_H__\n")
  (insert "\n")
  (insert "#endif /* __FILE_H__ */\n"))


;; ===========================
;; Emacs customized settings
;; ===========================
(custom-set-variables
  ;; custom-set-variables was added by Custom.
  ;; If you edit it by hand, you could mess it up, so be careful.
  ;; Your init file should contain only one such instance.
  ;; If there is more than one, they won't work right.
 '(all-christian-calendar-holidays t)
 '(calendar-week-start-day 1)
 '(case-fold-search t)
 '(case-replace t)
 '(column-number-mode t)
 '(compilation-auto-jump-to-first-error t)
 '(compilation-scroll-output (quote first-error))
 '(confirm-kill-emacs (quote y-or-n-p))
 '(cua-enable-cua-keys (quote shift))
 '(cua-highlight-region-shift-only t)
 '(desktop-save-mode t)
 '(diff-default-read-only t)
 '(diff-mode-hook (quote (diff-make-unified)))
 '(diff-switches "-u")
 '(diff-update-on-the-fly nil)
 '(dired-ls-F-marks-symlinks t)
 '(display-time-24hr-format t)
 '(display-time-day-and-date nil)
 '(display-time-default-load-average nil)
 '(display-time-mail-file (quote none))
 '(ecb-directories-menu-user-extension-function nil)
 '(ecb-history-menu-user-extension-function nil)
 '(ecb-methods-menu-user-extension-function nil)
 '(ecb-options-version "2.27")
 '(ecb-sources-menu-user-extension-function nil)
 '(erc-nick "troglobit")
 '(erc-server "irc.labs.westermo.se")
 '(erc-user-full-name "Joachim Nilsson")
 '(european-calendar-style t)
 '(global-auto-revert-mode t)
 '(indent-tabs-mode nil)
 '(indicate-buffer-boundaries (quote ((top . left) (bottom . right))))
 '(indicate-empty-lines t)
 '(inhibit-startup-screen t)
 '(mark-holidays-in-calendar t)
 '(pc-select-selection-keys-only t)
 '(pc-selection-mode t nil (pc-select))
 '(save-place t nil (saveplace))
 '(scroll-bar-mode (quote right))
 '(server-mode t)
 '(server-window (quote switch-to-buffer-other-frame))
 '(sh-basic-offset 8)
 '(sh-indentation 8)
 '(sh-learn-basic-offset (quote usually))
 '(show-paren-mode t)
 '(text-mode-hook (quote (turn-on-auto-fill text-mode-hook-identify)))
 '(todoo-collapse-items t)
 '(todoo-indent-column 0)
 '(transient-mark-mode t)
 '(uniquify-buffer-name-style (quote forward) nil (uniquify))
 '(vc-consult-headers nil)
 '(vc-dired-recurse nil)
 '(vc-dired-terse-display nil)
 '(vc-display-status nil)
 '(version-control (quote never))
 '(wdired-enable nil)
 '(which-function-mode t)
 '(x-select-enable-clipboard t))
(custom-set-faces
  ;; custom-set-faces was added by Custom.
  ;; If you edit it by hand, you could mess it up, so be careful.
  ;; Your init file should contain only one such instance.
  ;; If there is more than one, they won't work right.
 )

(put 'downcase-region 'disabled nil)
