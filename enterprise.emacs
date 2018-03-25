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
;; Copyright (c) 2009-2016  Joachim Nilsson <troglobit@gmail.com>
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

;; Who am I?
(setq user-full-name "Joachim Nilsson")
(setq user-mail-address "troglobit@gmail.com")

;; Location for privately maintained packages.
(add-to-list 'load-path "~/.emacs.d/site-lisp")
(add-to-list 'load-path "~/.emacs.d/epresent")

;; Prevent accidental exit of Emacs
(fset 'yes-or-no-p 'y-or-n-p)
(setq confirm-kill-emacs (quote y-or-n-p))

;; Add MELPA to emacs packages
(require 'package) ;; You might already have this line
(add-to-list 'package-archives
	     '("melpa" . "https://melpa.org/packages/"))
(when (< emacs-major-version 24)
  ;; For important compatibility libraries like cl-lib
  (add-to-list 'package-archives '("gnu" . "http://elpa.gnu.org/packages/")))
(package-initialize) ;; You might already have this line

;; Save minibuffer history between sessions
(setq savehist-additional-variables
      '(search-ring regexp-search-ring)
      savehist-file "~/.emacs.d/savehist")
(savehist-mode t)

;; Use projectile to sort ibuffer
(add-hook 'ibuffer-hook
    (lambda ()
      (ibuffer-projectile-set-filter-groups)
      (unless (eq ibuffer-sorting-mode 'alphabetic)
        (ibuffer-do-sort-by-alphabetic))))

;; Initialize external packages, from Debian.
;; (let ((startup-file "/usr/share/emacs/site-lisp/debian-startup.el"))
;;   (if (and (or (not (fboundp 'debian-startup))
;;                (not (boundp  'debian-emacs-flavor)))
;;            (file-readable-p startup-file))
;;       (progn
;;         (load-file startup-file)
;;         (setq debian-emacs-flavor 'emacs)
;;         (mapcar '(lambda (f)
;;                    (and (not (string= (substring f -3) "/.."))
;;                         (file-directory-p f)
;;                         (add-to-list 'load-path f)))
;;                 (directory-files "/usr/share/emacs/site-lisp" t)))))

;; Epresent orig
(autoload 'epresent-run "epresent")
(add-hook 'org-mode-hook
	(function
	 (lambda ()
	   (setq truncate-lines nil)
	   (setq word-wrap t)
	   (define-key org-mode-map [f3]
	     'epresent-run)
	   )))

;; Always enable ggtags for C projects
(add-hook 'c-mode-common-hook
          (lambda ()
            (when (derived-mode-p 'c-mode 'c++-mode 'java-mode)
              (ggtags-mode 1))))

;; Enable markdown-mode
(autoload 'markdown-mode "markdown-mode"
   "Major mode for editing Markdown files" t)
(add-to-list 'auto-mode-alist '("\\.markdown\\'" . markdown-mode))
(add-to-list 'auto-mode-alist '("\\.md\\'" . markdown-mode))

;; (autoload 'gfm-mode "gfm-mode"
;;    "Major mode for editing GitHub Flavored Markdown files" t)
;; (add-to-list 'auto-mode-alist '("README\\.md\\'" . gfm-mode))

;; Fix dired listings ...
(setq dired-listing-switches "-laGh1v --group-directories-first")
;;(dired-turn-on-discover t)

;; Workaround missing dead keys (`~ etc.) in Unity
(require 'iso-transl)

;; Helpful little thing https://github.com/justbur/emacs-which-key
;;(require 'which-key)
;;(which-key-mode t)
;;(which-key-setup-side-window-right-bottom)

;; Helpful yasnippets
;;(yas-global-mode t)

;; For external editor plugin to Thunderbird
;;(require 'tbemail)

;; For Flex & Bison
(require 'bison-mode)

;; Beautify Emacs
;; Add fringes
(setq-default indicate-buffer-boundaries 'right)
(setq-default indicate-empty-lines t)
(setq-default frame-title-format '("%b - %F"))

;; Make modeline prettier
(set-face-attribute 'mode-line nil :box nil)
(set-face-attribute 'mode-line-inactive nil :box nil)
(set-face-attribute 'mode-line-highlight nil :box nil)

;; Prettier scroll bar than toolkit default
'(global-yascroll-bar-mode t)
'(yascroll:delay-to-hide nil)
'(yascroll:scroll-bar (quote (right-fringe left-fringe text-area)))

;; Change default theme
;;(add-to-list 'custom-theme-load-path "~/.emacs.d/themes/")
;;(load-theme 'radiance t)
;;(load-theme 'github t)
;;(load-theme 'troglobit-dark t)
;;(load-theme 'darcula t)

;; Show colors in Emacs when color codes are listed
;;(rainbow-mode t)

;; Nyan Cat buffer position :-)
;; (nyan-mode t)

;; Smart modeline
;; (setq sml/theme 'dark)
;; (setq sml/theme 'light)
;; (setq sml/theme 'respectful)
;;(setq sml/theme 'smart-mode-line-powerline)
;;(sml/setup)

;; ANSI colors in Emacs compilation buffer
;; http://stackoverflow.com/questions/3072648/cucumbers-ansi-colors-messing-up-emacs-compilation-buffer
(ignore-errors
  (require 'ansi-color)
  (defun my-colorize-compilation-buffer ()
    (when (eq major-mode 'compilation-mode)
      (ansi-color-apply-on-region compilation-filter-start (point-max))))
  (add-hook 'compilation-filter-hook 'my-colorize-compilation-buffer))

;; Complete-anything http://company-mode.github.io/
;;(require 'company)
;;(add-to-list 'company-backends 'company-c-headers)

;; http://github.com/k-talo/smooth-scroll.el
;; https://www.reddit.com/r/emacs/comments/3idv1c/is_it_possible_for_emacs_to_approach_the_fluidity/
;;(require 'smooth-scroll)
;;(smooth-scroll-mode t)
;;(global-set-key [(meta  down)]  'scroll-up-1)
;;(global-set-key [(meta  up)]    'scroll-down-1)
;; (global-set-key [(meta  left)]  'scroll-right-1)
;; (global-set-key [(meta  right)] 'scroll-left-1)

;; Pimp org-files
;; https://thraxys.wordpress.com/2016/01/14/pimp-up-your-org-agenda/
;; (use-package org-bullets
;;	     :ensure t
;;	     :init
;;	     (setq org-bullets-bullet-list
;;		   '("◉" "◎" "⚫" "○" "►" "◇"))
;;	     :config
;;	     (add-hook 'org-mode-hook (lambda () (org-bullets-mode 1))))

;; (setq org-todo-keywords '((sequence "☛ TODO(t)" "|" "✔ DONE(d)")
;;			  (sequence "⚑ WAITING(w)" "|")
;;			  (sequence "|" "✘ CANCELED(c)")))

;;; Neat gnome-terminal like F11 toggle between fullscreen and windowed.
(defun fullscreen ()
  (interactive)
  (set-frame-parameter nil 'fullscreen
		       (if (frame-parameter nil 'fullscreen) nil 'fullboth)))

(defun insert-date ()
  (interactive)
  (insert (format-time-string "%Y-%m-%dT%H:%M:%S")))

(defun other-window-backwards ()
  (interactive)
  (other-window -1))

;; Some useful keybindings, first are the function keys,
;; freely adapted after the Borland C IDE.
(setq initial-scratch-message    ";; Welcome to Troglobit Enterprise ¯\\_(ツ)_/¯ Emacs!
;;\n\n")
;; Key     Function
;;   F1    Info
;; C-F1    Woman
;;   F2    Save
;; C-F2    Save as
;;   F3    Open
;;   F4    ☞ Goto line
;; C-F4    ⌕ Search
;;   F5    Breakpoint (GDB)
;; C-F5    Watcher    (GDB)
;; S-F5    Insert date
;;   F6    ⟷ Windows
;; C-F6    ⟷ Frames
;;   F7    ← Error
;; S-F7    Next instr (GDB)
;;   F8    → Error
;; S-F8    Step instr (GDB)
;;   F9    ⚙ Compile
;; C-F9    ⚙ Compile
;; S-F9    Debugger   (GDB)
;;   F10   ☰ Menu
;;   F11   ▣ Fullscreen
;;   F12   Toggle Menu bar
;; C-F12   Toggle Tool bar
;; S-F12   Toggle GDB windows
;; M-F12   Toggle Scroll bars
;;
;; M-#      Calculator
;; C-Tab    → Next window
;; C-S-Tab  ← Prev window
;; C-+      Font Sz++
;; C--      Font Sz--
;;
;; C-t C-l  Insert ISC license header
;; C-t C-s  Insert signed-off-by (private)
;; C-t C-w  Insert signed-off-by (work)
;; C-t C-t  Insert file header   (work)
;; C-t C-b  Insert file foooter  (work)
;; C-t C-h  Insert func. header
;; C-t C-i  Insert include body  (work)
;;
;; C-x C-b  iBuffer
;; C-c m    Magit Status
;;\n")

(global-set-key [f1] 'info)
(global-set-key [C-f1] 'woman)        ;; Context help

(global-set-key [f2] 'save-buffer)    ;; Save current
(global-set-key [C-f2] 'write-file)   ;; Save as

(global-set-key [S-f3] 'find-file)
(global-set-key [S-f4] 'goto-line)
;;(global-set-key [f4]   'call-last-kbd-macro)
(global-set-key [C-f4] 'isearch-forward)
(global-set-key [f5] 'gdb-toggle-breakpoint)
(global-set-key [C-f5] 'gud-watch)
(global-set-key [S-f5] 'insert-date)

(global-set-key [f6]   'other-window)   ;;
(global-set-key [C-f6] 'other-frame)
(global-set-key [f7]   'previous-error)
;;(global-set-key [f7] 'gdb-next)   ;; Only set in GDB mode?
(global-set-key [S-f7] 'gdb-next)   ;;

(global-set-key [f8] 'next-error)
(global-set-key [S-f8] 'gdb-step)   ;; Only set in GDB mode?

(global-set-key [f9] 'compile)
(global-set-key [C-f9] 'compile)
(global-set-key [S-f9] 'gdb)

;; Usability keys for ThinkPad X1 Carbon and similar keyboards
(global-set-key [C-prior] 'beginning-of-buffer)
(global-set-key [C-next]  'end-of-buffer)


(when window-system
  (global-set-key [f11]   'fullscreen)
  (global-unset-key [f12])
  (global-set-key [f12]   'menu-bar-mode)
  (global-set-key [C-f12] 'tool-bar-mode)
  (global-set-key [M-f12] 'scroll-bar-mode))
(global-set-key [S-f12] 'gdb-many-windows)

(global-set-key (kbd "M-#") 'calculator)
(global-set-key '[C-tab] 'other-window)
(global-set-key '[C-iso-lefttab] 'other-window-backwards)
(global-set-key (kbd "C-+") 'text-scale-increase)
(global-set-key (kbd "C--") 'text-scale-decrease)

(defun insert-file-header () (interactive)
  (insert "/* \\\\/ Westermo - <FILE DESCRIPTION>\n")
  (insert " *\n")
  (insert " * Copyright (C) 2016  Westermo Teleindustri AB\n")
  (insert " *\n")
  (insert " * Author: Joachim Nilsson <joachim.nilsson@westermo.se>\n")
  (insert " *\n")
  (insert " * Description:\n")
  (insert " *\n")
  (insert " */\n"))

(defun insert-include-body () (interactive)
  (insert "/* \\\\/ Westermo - <FILE DESCRIPTION>\n")
  (insert " *\n")
  (insert " * Copyright (C) 2016  Westermo Teleindustri AB\n")
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
  (insert " *  indent-tabs-mode: t\n")
  (insert " *  c-file-style: \"linux\"\n")
  (insert " * End:\n")
  (insert " */\n"))

(defun insert-function-header () (interactive)
  (insert "/**\n")
  (insert " * function-name -- Short description\n")
  (insert " * @var1: Variable description\n")
  (insert " * @var2: Variable description\n")
  (insert " *\n")
  (insert " * Function description, longer.\n")
  (insert " *\n")
  (insert " * Returns:\n")
  (insert " * If non-void function, description of return value and what\n")
  (insert " * @errno may be set to.\n")
  (insert " */\n"))

(defun insert-isc-license () (interactive)
  (insert "/*\n")
  (insert " *\n")
  (insert " * Copyright (c) 2016  Joachim Nilsson <troglobit@gmail.com>\n")
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

(defun magit-toggle-scroll-to-top () (recenter-top-bottom 0))
(advice-add 'magit-toggle-section :after #'magit-toggle-scroll-to-top)

;;(require 'flx-ido)
;;(ido-mode 1)
;;(ido-everywhere 1)
;;(flx-ido-mode 1)
;; disable ido faces to see flx highlights.
;;(setq ido-enable-flex-matching t)
;;(setq ido-use-faces nil)

 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
(custom-set-variables
 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(ansi-color-faces-vector
   [default default default italic underline success warning error])
 '(ansi-color-names-vector
   ["#424242" "#EF9A9A" "#C5E1A5" "#FFEE58" "#64B5F6" "#E1BEE7" "#80DEEA" "#E0E0E0"])
 '(beacon-color "#ec4780")
 '(before-save-hook nil)
 '(c-default-style
   (quote
    ((c-mode . "linux")
     (c++-mode . "linux")
     (java-mode . "java")
     (awk-mode . "awk")
     (other . "gnu"))))
 '(column-number-mode t)
 '(company-abort-manual-when-too-short t)
 '(company-auto-complete (quote (quote company-explicit-action-p)))
 '(compilation-message-face (quote default))
 '(completion-ignored-extensions
   (quote
    (".o" "~" ".bin" ".lbin" ".so" ".a" ".ln" ".blg" ".bbl" ".elc" ".lof" ".glo" ".idx" ".lot" ".svn/" ".hg/" ".git/" ".bzr/" "CVS/" "_darcs/" "_MTN/" ".fmt" ".tfm" ".class" ".fas" ".lib" ".mem" ".x86f" ".sparcf" ".dfsl" ".pfsl" ".d64fsl" ".p64fsl" ".lx64fsl" ".lx32fsl" ".dx64fsl" ".dx32fsl" ".fx64fsl" ".fx32fsl" ".sx64fsl" ".sx32fsl" ".wx64fsl" ".wx32fsl" ".fasl" ".ufsl" ".fsl" ".dxl" ".lo" ".la" ".gmo" ".mo" ".toc" ".aux" ".cp" ".fn" ".ky" ".pg" ".tp" ".vr" ".cps" ".fns" ".kys" ".pgs" ".tps" ".vrs" ".pyc" ".pyo" ".d")))
 '(custom-enabled-themes (quote (atom-dark)))
 '(custom-safe-themes
   (quote
    ("e9460a84d876da407d9e6accf9ceba453e2f86f8b86076f37c08ad155de8223c" "5a0eee1070a4fc64268f008a4c7abfda32d912118e080e18c3c865ef864d1bea" "c3e6b52caa77cb09c049d3c973798bc64b5c43cc437d449eacf35b3e776bf85c" "5e52ce58f51827619d27131be3e3936593c9c7f9f9f9d6b33227be6331bf9881" default)))
 '(delete-active-region t)
 '(delete-selection-mode t)
 '(desktop-restore-in-current-display t)
 '(desktop-save-mode t)
 '(diff-switches "-u")
 '(display-battery-mode t)
 '(ediff-window-setup-function (quote ediff-setup-windows-plain))
 '(evil-emacs-state-cursor (quote ("#E57373" hbar)))
 '(evil-insert-state-cursor (quote ("#E57373" bar)))
 '(evil-normal-state-cursor (quote ("#FFEE58" box)))
 '(evil-visual-state-cursor (quote ("#C5E1A5" box)))
 '(fci-rule-color "#383838")
 '(fill-column 72)
 '(font-use-system-font nil)
 '(global-auto-revert-mode t)
 '(global-company-mode nil)
 '(global-magit-file-mode t)
 '(graphviz-dot-preview-extension "svg")
 '(highlight-indent-guides-auto-enabled nil)
 '(highlight-symbol-colors
   (quote
    ("#FFEE58" "#C5E1A5" "#80DEEA" "#64B5F6" "#E1BEE7" "#FFCC80")))
 '(highlight-symbol-foreground-color "#E0E0E0")
 '(highlight-tail-colors (quote (("#ec4780" . 0) ("#424242" . 100))))
 '(ido-mode (quote both) nil (ido))
 '(indicate-buffer-boundaries (quote ((top . left) (bottom . right))))
 '(indicate-empty-lines t)
 '(inhibit-startup-screen t)
 '(ispell-dictionary "american")
 '(magit-commit-arguments (quote ("--signoff")))
 '(magit-commit-signoff t)
 '(magit-diff-refine-hunk (quote all))
 '(magit-diff-use-overlays nil)
 '(nrepl-message-colors
   (quote
    ("#CC9393" "#DFAF8F" "#F0DFAF" "#7F9F7F" "#BFEBBF" "#93E0E3" "#94BFF3" "#DC8CC3")))
 '(org-fontify-done-headline t)
 '(org-fontify-quote-and-verse-blocks t)
 '(org-fontify-whole-heading-line t)
 '(mouse-wheel-scroll-amount (quote (1 ((shift) . 1) ((control)))))
 '(org-support-shift-select t)
 '(package-selected-packages
   (quote
    (atom-dark-theme apropospriate-theme centered-cursor-mode zenburn-theme spotify rtags popup-switcher markdown-mode magit lua-mode langtool ibuffer-projectile helm-gtags helm-git go-mode git-gutter-fringe gist ggtags flycheck flx-ido flim f dockerfile-mode discover debian-changelog-mode dash-functional company-c-headers ag)))
 '(pdf-view-midnight-colors (quote ("#DCDCCC" . "#383838")))
 '(pos-tip-background-color "#3a3a3a")
 '(pos-tip-foreground-color "#9E9E9E")
 '(projectile-mode t t (projectile))
 '(scroll-bar-mode nil)
 '(scroll-conservatively 10000)
 '(server-kill-new-buffers t)
 '(server-mode t)
 '(show-paren-mode t)
 '(split-height-threshold 200)
 '(split-width-threshold 140)
 '(tool-bar-mode nil)
 '(tooltip-mode nil)
 '(tramp-syntax (quote default))
 '(uniquify-buffer-name-style (quote forward) nil (uniquify))
 '(vc-annotate-background "#2B2B2B")
 '(vc-annotate-color-map
   (quote
    ((20 . "#BC8383")
     (40 . "#CC9393")
     (60 . "#DFAF8F")
     (80 . "#D0BF8F")
     (100 . "#E0CF9F")
     (120 . "#F0DFAF")
     (140 . "#5F7F5F")
     (160 . "#7F9F7F")
     (180 . "#8FB28F")
     (200 . "#9FC59F")
     (220 . "#AFD8AF")
     (240 . "#BFEBBF")
     (260 . "#93E0E3")
     (280 . "#6CA0A3")
     (300 . "#7CB8BB")
     (320 . "#8CD0D3")
     (340 . "#94BFF3")
     (360 . "#DC8CC3"))))
 '(vc-annotate-very-old-color "#DC8CC3")
 '(which-function-mode t))

;; Envy Code R is usally a great font, other great fonts are
;; Source Code Pro and Inconsolata
(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(default ((t (:family "Envy Code R" :foundry "ENVY" :slant normal :weight normal :height 98 :width normal)))))
