;; Troglobit Enterprise ¯\_(ツ)_/¯ Emacs                -*-Emacs-LISP-*-
;; 
;; This used to be a holy grail I called EnterpriseEmacs, but these days
;; anyyone can make use of Emacs in so many ways.  The best instrument
;; I've ever used and I keep learning more every day!
;;
;; Emacs is my daily driver, my preferred instrument.  I'm completely
;; addicted to ido-mode, magit, org-mode, and many more exclusive
;; features of Emacs.
;;
;; This file can be freely used, reused, modified and distributed under
;; the terms of the ISC License.
;;
;; Save it as ~/.emacs (dot emacs in your home directory) to activate
;; and then (re)start your Emacs.
;;
;; Copyright (c) 2009-2015  Joachim Nilsson <troglobit@gmail.com>
;;
;; Permission to use, copy, modify, and/or distribute this software for any
;; purpose with or without fee is hereby granted, provided that the above
;; copyright notice and this permission notice appear in all copies.
;;
;; THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
;; WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
;; MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
;; ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
;; WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
;; ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
;; OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

;; Location for privately maintained packages.
(add-to-list 'load-path "~/.emacs.d/site-lisp")
;;(load-file "~/.emacs.d/site-lisp/graphviz-dot-mode.el")
;;(autoload 'cflow-mode "cflow-mode")
;;(setq auto-mode-alist (append auto-mode-alist
;;                              '(("\\.cflow$" . cflow-mode))))

;; Location for external plugins
;;(add-to-list 'load-path "~/.emacs.d/plugins/yasnippet-0.6.1c")
;;(require 'yasnippet)
;;(yas/initialize)
;;(yas/load-directory "~/.emacs.d/plugins/yasnippet-0.6.1c/snippets")

;;; Initialize external packages, from Debian.
(let ((startup-file "/usr/share/emacs/site-lisp/debian-startup.el"))
  (if (and (or (not (fboundp 'debian-startup))
               (not (boundp  'debian-emacs-flavor)))
           (file-readable-p startup-file))
      (progn
        (load-file startup-file)
        (setq debian-emacs-flavor 'emacs)
        (mapcar '(lambda (f)
                   (and (not (string= (substring f -3) "/.."))
                        (file-directory-p f)
                        (add-to-list 'load-path f)))
                (directory-files "/usr/share/emacs/site-lisp" t)))))

;; XKCD Reader :-)
(add-to-list 'load-path (expand-file-name "/home/jocke/Troglobit/emacs-xkcd"))
(require 'xkcd)

;; Emacs Code Browser
;;(require 'cedet)
;;(require 'ecb)

;;; Autoload Stefan Reichoer's cool psvn
;;(require 'psvn)

;;; Autoload magit
;;(require 'magit)

;;; Autoload SCO UNIX Cscope
;;(require 'xcscope)

;; Setup GNU Global code indexing
;;(autoload 'gtags-mode "gtags" "" t)
;;(setq c-mode-hook
;;  '(lambda ()
;;     (gtags-mode 1)))

;; Cedet now part of Emacs
;;(global-ede-mode 1)                      ; Enable the Project management system
;;(semantic-load-enable-code-helpers)      ; Enable prototype help and smart completion

;;(push '(font-backend xft x) default-frame-alist)

;; window maximized
;;(setq default-frame-alist '((top . 1) (left . 1) (width . 132) (height . 40)))

;; Window transparency
;;(set-frame-parameter (selected-frame) 'alpha '(<active> [<inactive>]))
;(set-frame-parameter (selected-frame) 'alpha '(90 90))
;(add-to-list 'default-frame-alist '(alpha 90 90))

;;; Neat gnome-terminal like F11 toggle between fullscreen and windowed.
(defun fullscreen ()
  (interactive)
  (set-frame-parameter nil 'fullscreen
                       (if (frame-parameter nil 'fullscreen) nil 'fullboth)))

;; For Emacs 23, or older
;;(require 'color-theme)
;;(eval-after-load "color-theme"
;;  '(progn
;;     (color-theme-initialize)
;;     (color-theme-hober)))

;; For Emacs 24, or newer
(require 'package)
(add-to-list 'package-archives 
             '("marmalade" .
               "http://marmalade-repo.org/packages/"))
(package-initialize)
;;(load-theme 'solarized-dark t)

;; Emacs Multi Media System
;;(require 'emms-setup)
;;(require 'emms-browser)
;;(emms-all)
;;(emms-default-players)
;;(setq emms-source-file-default-directory "~/Music/")

;;; Convert CamelCase() to camel_case()
(defun un-camelcase-string (s &optional sep start)
  "Convert CamelCase string S to lower case with word separator SEP.
   Default for SEP is an underscore \"_\".

   If third argument START is non-nil, convert words after that
   index in STRING."
  (let ((case-fold-search nil))
    (while (string-match "[A-Z]" s (or start 1))
      (setq s (replace-match (concat (or sep "_")
                                     (downcase (match-string 0 s)))
                             t nil s)))
    (downcase s)))

;; nuke whitespaces when writing to a file
;;(add-hook 'before-save-hook 'whitespace-cleanup)

;; http://info.borland.com/techpubs/jbuilder/jbuilder9/introjb/key_cua.html
;; These are the bindings also used in early Borland C++ versions, like 3.0
;; the last known "good" release.
; function keys
;; 1help, 2save, 3load, 4goto, 5zoom, 6next, 7split, 8exit!, 9make, 10other, 11fullscrn, 12undo
;;       C2save as,                                         C9Build
;;                                                          S9debug
;; emacs: must use command name to allow interactive input

(global-set-key [f1] 'info)
(global-set-key [C-f1] 'woman)        ;; Context help

(global-set-key [f2] 'save-buffer)    ;; Save current
(global-set-key [C-f2] 'write-file)   ;; Save as

(global-set-key [f3] 'find-file)
(global-set-key [f4] 'goto-line)
(global-set-key [F5] 'gdb-toggle-breakpoint)
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

(global-set-key [f11] 'fullscreen)

(global-set-key (kbd "M-#") 'calculator)
;;(global-set-key (kbd "C-c c") 'calculator)
;;
(global-set-key [C-prior] 'beginning-of-buffer)
(global-set-key [C-next] 'end-of-buffer)

;;; Key binding for switching to next and previous buffer
;;(global-set-key '[C-tab] 'bs-cycle-next)
;;(global-set-key '[C-S-iso-lefttab] 'bs-cycle-previous)
(global-set-key '[C-tab] 'other-window)
(global-set-key '[C-S-iso-lefttab] 'bs-cycle-previous)

;; Johans hack with dabbrev
(global-set-key [backtab]    'dabbrev-expand)

;; Fix problem with s-dead-circumflex and other issues where using
;; compose to get ^ ` et consortes no longer worked for me in Ubuntu.
(require 'iso-transl)

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

(c-add-style "openbsd"
             '("bsd"
               (indent-tabs-mode . t)
               (defun-block-intro . 8)
               (statement-block-intro . 8)
               (statement-case-intro . 8)
               (substatement-open . 4)
               (substatement . 8)
               (arglist-cont-nonempty . 4)
               (inclass . 8)
               (knr-argdecl-intro . 8)))

(c-add-style "mrouted"
             '("ellemtel"
               (c-basic-offset . 4)))

 (c-add-style "microsoft"
              '("stroustrup"
                (c-offsets-alist
                 (innamespace . -)
                 (inline-open . 0)
                 (inher-cont . c-lineup-multi-inher)
                 (arglist-cont-nonempty . +)
                 (template-args-cont . +))))

;; Add color to a shell running in emacs 'M-x shell'
(autoload 'ansi-color-for-comint-mode-on "ansi-color" nil t)
(add-hook 'shell-mode-hook 'ansi-color-for-comint-mode-on)

;; ============================
;; Mouse Settings
;; ============================

;; mouse button one drags the scroll bar
;;(global-set-key [vertical-scroll-bar down-mouse-1] 'scroll-bar-drag)

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
;;(setq scroll-preserve-screen-position 1)

;; don't automatically add new lines when scrolling down at
;; the bottom of a buffer
(setq next-line-add-newlines nil)

;; scroll just one line when hitting the bottom of the window
;;(setq scroll-step 1)
;;(setq scroll-conservatively 1)

;; format the title-bar to always include the buffer name
(setq frame-title-format "emacs - %b")

;; show a menu only when running within X (save real estate when
;; in console)
(menu-bar-mode (if window-system 1 -1))

;; replace highlighted text with what I type rather than just
;; inserting at a point
(delete-selection-mode t)

;; resize the mini-buffer when necessary
(setq resize-minibuffer-mode t)

;; highlight during searching
(setq query-replace-highlight t)

;; highlight incremental search
(setq search-highlight t)

;; Fully redraw the display before it processes queued input events.
;; http://www.masteringemacs.org/articles/2011/10/02/improving-performance-emacs-display-engine/
(setq redisplay-dont-pause t)

;; Disable question about killing a buffer with a live process attached to it
(setq kill-buffer-query-functions
  (remq 'process-kill-buffer-query-function
         kill-buffer-query-functions))

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

(defun insert-function-header () (interactive)
  (insert "/**\n")
  (insert " * function - Short description.\n")
  (insert " * @param: Description\n")
  (insert " * \n")
  (insert " * Returns: \n")
  (insert " * \n")
  (insert " */\n"))

(defun insert-file-header () (interactive)
  (insert "/* \\\\/ Westermo - <FILE DESCRIPTION>\n")
  (insert " *\n")
  (insert " * Copyright (C) 2015  Westermo Teleindustri AB\n")
  (insert " *\n")
  (insert " * Author: Joachim Nilsson <joachim.nilsson@westermo.se>\n")
  (insert " *\n")
  (insert " * Description:\n")
  (insert " *\n")
  (insert " */\n"))

(defun insert-include-body () (interactive)
  (insert "/* \\\\/ Westermo - <FILE DESCRIPTION>\n")
  (insert " *\n")
  (insert " * Copyright (C) 2015  Westermo Teleindustri AB\n")
  (insert " *\n")
  (insert " * Author: Joachim Nilsson <joachim.nilsson@westermo.se>\n")
  (insert " *\n")
  (insert " * Description:\n")
  (insert " *\n")
  (insert " */\n")
  (insert "\n")
  (insert "#ifndef FILE_H_\n")
  (insert "#define FILE_H_\n")
  (insert "\n")
  (insert "#endif /* FILE_H_ */\n"))

(defun insert-file-footer () (interactive)
  (insert "/**\n")
  (insert " * Local Variables:\n")
  (insert " *  version-control: t\n")
  (insert " *  indent-tabs-mode: nil\n")
  (insert " *  c-file-style: \"ellemtel\"\n")
  (insert " * End:\n")
  (insert " */\n"))

(defun insert-isc-license () (interactive)
  (insert "/*\n")
  (insert " *\n")
  (insert " * Copyright (c) 2015  Joachim Nilsson <troglobit@gmail.com>\n")
  (insert " *\n")
  (insert " * Permission to use, copy, modify, and/or distribute this software for any\n")
  (insert " * purpose with or without fee is hereby granted, provided that the above\n")
  (insert " * copyright notice and this permission notice appear in all copies.\n")
  (insert " *\n")
  (insert " * THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES\n")
  (insert " * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF\n")
  (insert " * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR\n")
  (insert " * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES\n")
  (insert " * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN\n")
  (insert " * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF\n")
  (insert " * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.\n")
  (insert " */\n"))

(defun insert-wmo-signed-off () (interactive)
  (insert "Signed-off-by: Joachim Nilsson <joachim.nilsson@westermo.se>\n"))

(defun insert-signed-off () (interactive)
  (insert "Signed-off-by: Joachim Nilsson <troglobit@gmail.com>\n"))

;; insert functions
(global-unset-key "\C-t")
(global-set-key "\C-t\C-l" 'insert-isc-license)     ; Alternate top
(global-set-key "\C-t\C-w" 'insert-wmo-signed-off)
(global-set-key "\C-t\C-s" 'insert-signed-off)
(global-set-key "\C-t\C-t" 'insert-file-header)     ; Top
(global-set-key "\C-t\C-b" 'insert-file-footer)     ; Bottom
(global-set-key "\C-t\C-h" 'insert-function-header) ; Header
(global-set-key "\C-t\C-i" 'insert-include-body)    ; Include

;; Override buffer listing with new ibuffer
(global-set-key (kbd "C-x C-b") 'ibuffer)

;; Bind C-c m to magit-status
(global-set-key (kbd "C-c m") 'magit-status)

;; Transparency, from http://emacs-fu.blogspot.se/2009/02/transparent-emacs.html
(defun djcb-opacity-modify (&optional dec)
  "modify the transparency of the emacs frame; if DEC is t,
    decrease the transparency, otherwise increase it in 10%-steps"
  (let* ((alpha-or-nil (frame-parameter nil 'alpha)) ; nil before setting
          (oldalpha (if alpha-or-nil alpha-or-nil 100))
          (newalpha (if dec (- oldalpha 10) (+ oldalpha 10))))
    (when (and (>= newalpha frame-alpha-lower-limit) (<= newalpha 100))
      (modify-frame-parameters nil (list (cons 'alpha newalpha))))))

 ;; C-8 will increase opacity (== decrease transparency)
 ;; C-9 will decrease opacity (== increase transparency
 ;; C-0 will returns the state to normal
(global-set-key (kbd "C-8") '(lambda()(interactive)(djcb-opacity-modify)))
(global-set-key (kbd "C-9") '(lambda()(interactive)(djcb-opacity-modify t)))
(global-set-key (kbd "C-0") '(lambda()(interactive)
                               (modify-frame-parameters nil `((alpha . 100)))))

;; Language tool -- https://github.com/mhayashi1120/Emacs-langtool
(require 'langtool)
(setq langtool-mother-tongue "en")
(setq langtool-language-tool-jar "~/LanguageTool-2.7/languagetool-commandline.jar")
(global-set-key "\C-x4w" 'langtool-check)
(global-set-key "\C-x4W" 'langtool-check-done)
(global-set-key "\C-x4l" 'langtool-switch-default-language)
(global-set-key "\C-x44" 'langtool-show-message-at-point)
(global-set-key "\C-x4c" 'langtool-correct-buffer)

;; All settings activated and saved in the Options menu Emacs stores here
(custom-set-variables
 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(all-christian-calendar-holidays t)
 '(ansi-color-names-vector [solarized-bg red green yellow blue magenta cyan solarized-fg])
 '(auto-hscroll-mode t)
 '(baud-rate 115200)
 '(c-default-style (quote ((c-mode . "linux") (c++-mode . "ellemtel") (java-mode . "java") (awk-mode . "awk") (other . "gnu"))))
 '(c-max-one-liner-length 132)
 '(calendar-christian-all-holidays-flag t)
 '(calendar-mark-holidays-flag t)
 '(calendar-week-start-day 1)
 '(case-fold-search t)
 '(case-replace t)
 '(column-number-mode t)
 '(compilation-auto-jump-to-first-error nil)
 '(compilation-scroll-output t)
 '(compilation-window-height 15)
 '(confirm-kill-emacs (quote y-or-n-p))
 '(cua-enable-cua-keys (quote shift))
 '(cua-highlight-region-shift-only t)
 '(cua-mode t nil (cua-base))
 '(custom-enabled-themes (quote (tango-dark)))
 '(custom-safe-themes (quote ("72cc9ae08503b8e977801c6d6ec17043b55313cda34bcf0e6921f2f04cf2da56" "d2622a2a2966905a5237b54f35996ca6fda2f79a9253d44793cfe31079e3c92b" "501caa208affa1145ccbb4b74b6cd66c3091e41c5bb66c677feda9def5eab19c" default)))
 '(desktop-save-mode t)
 '(diff-default-read-only nil)
 '(diff-mode-hook (quote (diff-make-unified)))
 '(diff-switches "-u")
 '(diff-update-on-the-fly nil)
 '(dired-ls-F-marks-symlinks t)
 '(display-battery-mode t)
 '(display-time-24hr-format t)
 '(display-time-day-and-date nil)
 '(display-time-default-load-average nil)
 '(display-time-mail-file (quote none))
 '(ecb-directories-menu-user-extension-function (quote ignore))
 '(ecb-history-menu-user-extension-function (quote ignore))
 '(ecb-layout-name "leftright2")
 '(ecb-methods-menu-user-extension-function (quote ignore))
 '(ecb-options-version "2.32")
 '(ecb-primary-secondary-mouse-buttons (quote mouse-1--mouse-2))
 '(ecb-sources-menu-user-extension-function (quote ignore))
 '(ecb-tip-of-the-day nil)
 '(ecb-vc-supported-backends (quote ((ecb-vc-dir-managed-by-CVS . ecb-vc-state) (ecb-vc-dir-managed-by-RCS . ecb-vc-state) (ecb-vc-dir-managed-by-SVN . ecb-vc-state))))
 '(ecb-windows-width 0.13)
 '(ede-project-directories (quote ("/home/jocke/Troglobit/uftpd")))
 '(ediff-merge-split-window-function (quote split-window-vertically))
 '(erc-nick "troglobit")
 '(erc-server "irc.labs.westermo.se")
 '(erc-user-full-name "Joachim Nilsson")
 '(european-calendar-style t)
 '(fci-rule-color "#eee8d5")
 '(fill-column 72)
 '(gdb-same-frame t)
 '(gdb-show-main t)
 '(global-auto-revert-mode t)
 '(global-hl-line-mode nil)
 '(hide-ifdef-lines t)
 '(hide-ifdef-shadow t)
 '(ido-enabled (quote both) nil (ido))
 '(ido-everywhere t)
 '(ido-mode (quote both) nil (ido))
 '(indent-tabs-mode nil)
 '(indicate-buffer-boundaries (quote ((top . left) (bottom . right))))
 '(indicate-empty-lines t)
 '(inhibit-startup-screen t)
 '(make-backup-files nil)
 '(mark-holidays-in-calendar t)
 '(menu-bar-mode nil)
 '(org-support-shift-select t)
 '(pc-select-selection-keys-only t)
 '(pc-selection-mode t)
 '(safe-local-variable-values (quote ((c-style . pluto) (c-indent-style . "ellemtel"))))
 '(save-place t nil (saveplace))
 '(scroll-bar-mode nil)
 '(server-done-hook (quote (delete-frame)))
 '(server-mode t)
 '(server-window (quote switch-to-buffer-other-frame))
 '(sh-basic-offset 8)
 '(sh-indentation 8)
 '(sh-learn-basic-offset (quote usually))
 '(show-paren-mode t)
 '(text-mode-hook (quote (turn-on-auto-fill text-mode-hook-identify)))
 '(todoo-collapse-items t)
 '(todoo-indent-column 0)
 '(tool-bar-mode nil)
 '(tooltip-mode nil)
 '(truncate-lines t)
 '(uniquify-buffer-name-style (quote forward) nil (uniquify))
 '(vc-annotate-background "white")
 '(vc-consult-headers nil)
 '(vc-dired-recurse nil)
 '(vc-dired-terse-display nil)
 '(vc-display-status nil)
 '(version-control (quote never))
 '(wdired-enable nil)
 '(which-function-mode t)
 '(x-select-enable-clipboard t))

;; The font 'Envy Code R' is made by Damien Guard, freely available from:
;; http://damieng.com/blog/2008/05/26/envy-code-r-preview-7-coding-font-released
(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(default ((t (:family "Envy Code R" :foundry "unknown" :slant normal :weight normal :height 113 :width normal)))))
