;; Troglobit Enterprise ¯\_(ツ)_/¯ Emacs                -*-Emacs-LISP-*-
;;
;; This used to be a holy grail I called EnterpriseEmacs.  These days
;; Emacs is so much more user friendly, yet still the best instrument
;; I've ever used, and learn from every day!
;;
;; Emacs is my daily driver, my preferred workbench.  I'm completely
;; addicted to ido-mode, magit, org-mode, and many more exclusive
;; features of Emacs.
;;
;; This file can be freely used, reused, modified and distributed under
;; the terms of the ISC License.
;;
;; Save it as ~/.emacs (dot emacs in your home directory) to activate
;; and then (re)start your Emacs to start populating ~/.emacs.d/ ...
;;
;; Copyright (c) 2009-2019  Joachim Nilsson <troglobit@gmail.com>
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

(setq initial-scratch-message    ";; Welcome to Troglobit Enterprise ¯\\_(ツ)_/¯ Emacs!
;;\n\n")

;; Who am I?
(setq user-full-name "Joachim Nilsson")
(setq user-mail-address "troglobit@gmail.com")

;; Location for privately maintained packages.
(add-to-list 'load-path "~/.emacs.d/site-lisp")
(add-to-list 'load-path "~/.emacs.d/epresent")

;; Prevent accidental exit of Emacs
(fset 'yes-or-no-p 'y-or-n-p)
(setq confirm-kill-emacs (quote y-or-n-p))

;; Setup MacBook cmd key as Meta
(setq mac-option-modifier 'super)
(setq mac-command-modifier 'meta)

;;; package setup
(setq load-prefer-newer t
      use-package-always-ensure t
      package-enable-at-startup nil
      package-archives
      '(("gnu" . "https://elpa.gnu.org/packages/")
        ("org" . "http://orgmode.org/elpa/")
        ("melpa" . "https://melpa.org/packages/")))

(require 'package)
(package-initialize)

;; Bootstrap `use-package': if not installed, refresh remotes, install it.
;; https://github.com/jwiegley/use-package
(unless (package-installed-p 'use-package)
  (package-refresh-contents)
  (package-install 'use-package))

;; for now accept that this is magic
(eval-when-compile
  (require 'use-package))

;; Save minibuffer history between sessions
(setq savehist-additional-variables
      '(search-ring regexp-search-ring)
      savehist-file "~/.emacs.d/savehist")
(savehist-mode t)

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

;; When saving a file that starts with #!, make it executable.
(add-hook 'after-save-hook
          'executable-make-buffer-file-executable-if-script-p)

;; Fix dired listings ...
(setq dired-listing-switches "-laGh1v --group-directories-first")
;;(dired-turn-on-discover t)

;; Enable recentf-mode and remember a lot of files.
(recentf-mode 1)
(defvar recentf-max-saved-items)
(setq recentf-max-saved-items 200)

;; Workaround missing dead keys (`~ etc.) in Unity
(require 'iso-transl)

;; PACKAGES ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(require 'use-package)

(use-package gruvbox-theme
  :ensure t
  :init  (load-theme 'gruvbox-dark-medium t))

;;
(use-package ido-completing-read+
  :ensure t)

;; Smart M-x for Emacs built on top of ido mode
(use-package smex
  :init
  (global-set-key (kbd "M-x") 'smex))

;; Multiple terminal manager for Emacs
(use-package multi-term
  :init
  (setq multi-term-program "/bin/bash"))
(require 'multi-term)

;; Mode for Git Commits
(use-package git-commit)

;; Command to clean up and prettify json.
(use-package json-mode)

(use-package markdown-mode
  :ensure t
  :commands (markdown-mode gfm-mode)
  :hook (markdown-mode . flyspell-mode)
  :mode (("README\\.md\\'" . gfm-mode)
         ("\\.md\\'" . markdown-mode)
         ("\\.markdown\\'" . markdown-mode))
  :init (setq markdown-command "pandoc"))

;; Auto-detect indent settings
(use-package dtrt-indent
  :ensure t)

(use-package flycheck
  :ensure t
  :init (global-flycheck-mode))

(use-package flycheck-clang-tidy
  :after flycheck
  :hook (flycheck-mode . flycheck-clang-tidy-setup))

;; Directory sidebar
;; (use-package treemacs
;;   :bind
;;   (("C-c t" . treemacs)
;;    ("s-a" . treemacs)))

;; GNU Global Tags
(use-package ggtags
  :ensure t
  :commands ggtags-mode
  :diminish ggtags-mode
  :bind (("M-*" . pop-tag-mark)
         ("C-c t s" . ggtags-find-other-symbol)
         ("C-c t h" . ggtags-view-tag-history)
         ("C-c t r" . ggtags-find-reference)
         ("C-c t f" . ggtags-find-file)
         ("C-c t c" . ggtags-create-tags))
  :init
  (add-hook 'c-mode-common-hook
            #'(lambda ()
                (when (derived-mode-p 'c-mode 'c++-mode 'java-mode)
                  (ggtags-mode 1)))))

;; Complete anything (company)
(use-package company
  :ensure t
  :defer t
  :init (global-company-mode)
  :config
  (progn
    ;; Use Company for completion
    (bind-key [remap completion-at-point] #'company-complete company-mode-map)

    (setq company-tooltip-align-annotations t
          ;; Easy navigation to candidates with M-<n>
          company-show-numbers t)
    (setq company-dabbrev-downcase nil))
  :diminish company-mode)

(use-package company-quickhelp          ; Documentation popups for Company
  :ensure t
  :defer t
  :init (add-hook 'global-company-mode-hook #'company-quickhelp-mode))

(use-package company-c-headers
  :ensure t
  :defer t
  :init
  (with-eval-after-load 'company
    (add-to-list 'company-backends 'company-c-headers)))

;; Powerline is a neat modeline replacement
(use-package powerline
  :ensure t
  :init (powerline-default-theme))

;; It's useful to be able to restart emacs from inside emacs.
(use-package restart-emacs)

;; Project Handling
(use-package projectile
  :ensure t
  :init
  :bind-keymap (("C-c p" . projectile-command-map))
  :config (projectile-global-mode t))

(use-package ibuffer-projectile)
(use-package ibuffer
  ;; A different buffer view.
  :bind ("C-x C-b" . ibuffer)
  :init
  (progn
    (require 'ibuf-ext)

    (setq ibuffer-filter-group-name-face 'success) ; TODO: declare it's own face.
    (setq ibuffer-show-empty-filter-groups nil)
    (add-to-list 'ibuffer-never-show-predicates "^\\*")

    (add-hook 'ibuffer-mode-hooks 'ibuffer-auto-mode)

    (setq ibuffer-formats
          '((mark modified read-only " "
		  (name 18 18 :left :elide) " "
		  (size 9 -1 :right) " "
		  (mode 16 16 :left :elide) " " filename-and-process)
            (mark " " (name 16 -1) " " filename)))))

;; Use projectile to sort ibuffer
(add-hook 'ibuffer-hook
    (lambda ()
      (ibuffer-projectile-set-filter-groups)
      (unless (eq ibuffer-sorting-mode 'alphabetic)
        (ibuffer-do-sort-by-alphabetic))))

;; magit
(use-package magit
	     :commands (magit-status projectile-vc)
	     :bind (("C-c m" . magit-status)
		    ("C-c l" . magit-log-buffer-file)
		    ("C-c M-g" . magit-dispatch-popup))
	     :config (use-package ido-completing-read+)
	     (setq magit-popup-use-prefix-argument 'default
		   magit-completing-read-function 'magit-ido-completing-read
		   global-magit-file-mode 't
		   magit-commit-arguments (quote ("--signoff"))
		   magit-commit-signoff 't
		   magit-diff-refine-hunk (quote all)
		   magit-diff-use-overlays nil))

(use-package magit-popup)

;; This package will display all available keybindings in a popup.
;; https://github.com/justbur/emacs-which-key
;; (use-package which-key
;;   :diminish which-key-mode
;;   :config
;;   (which-key-mode)
;;   (which-key-setup-side-window-bottom)
;;   (setq which-key-use-C-h-for-paging t
;; 	which-key-prevent-C-h-from-cycling t))

;; Helpful little thing https://github.com/justbur/emacs-which-key
;;(require 'which-key)
;;(which-key-mode t)
;;(which-key-setup-side-window-right-bottom)

;; Helpful yasnippets
;;(yas-global-mode t)

;; For external editor plugin to Thunderbird
;;(require 'tbemail)

;; For Flex & Bison
;;(require 'bison-mode)

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

;; For Emacs 27.1 tab-bar-mode
;; (global-set-key '[C-next] 'tab-next)
;; (global-set-key '[C-prior] 'tab-previous)

(defun insert-file-header () (interactive)
  (insert "/* \\\\/ Westermo - <FILE DESCRIPTION>\n")
  (insert " *\n")
  (insert " * Copyright (C) 2019  Westermo Network Technologies AB\n")
  (insert " *\n")
  (insert " * Author: Joachim Nilsson <joachim.nilsson@westermo.se>\n")
  (insert " *\n")
  (insert " * Description:\n")
  (insert " *\n")
  (insert " */\n"))

(defun insert-include-body () (interactive)
  (insert "/* \\\\/ Westermo - <FILE DESCRIPTION>\n")
  (insert " *\n")
  (insert " * Copyright (C) 2019  Westermo Network Technologies\n")
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
  (insert " * Copyright (c) 2019  Joachim Nilsson <troglobit@gmail.com>\n")
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
 '(custom-enabled-themes (quote (gruvbox-dark-medium)))
 '(custom-safe-themes
   (quote
    ("84d2f9eeb3f82d619ca4bfffe5f157282f4779732f48a5ac1484d94d5ff5b279" "a22f40b63f9bc0a69ebc8ba4fbc6b452a4e3f84b80590ba0a92b4ff599e53ad0" "585942bb24cab2d4b2f74977ac3ba6ddbd888e3776b9d2f993c5704aa8bb4739" "8f97d5ec8a774485296e366fdde6ff5589cf9e319a584b845b6f7fa788c9fa9a" "669e02142a56f63861288cc585bee81643ded48a19e36bfdf02b66d745bcc626" default)))
 '(delete-active-region t)
 '(delete-selection-mode t)
 '(desktop-restore-in-current-display t)
 '(desktop-save-mode t)
 '(diff-switches "-u")
 '(ediff-window-setup-function (quote ediff-setup-windows-plain))
 '(fci-rule-color "#383838")
 '(fill-column 72)
 '(font-use-system-font nil)
 '(global-auto-revert-mode t)
 '(global-company-mode nil)
 '(global-magit-file-mode t)
 '(global-prettify-symbols-mode t)
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
 '(mouse-wheel-scroll-amount (quote (1 ((shift) . 1) ((control)))))
 '(nrepl-message-colors
   (quote
    ("#CC9393" "#DFAF8F" "#F0DFAF" "#7F9F7F" "#BFEBBF" "#93E0E3" "#94BFF3" "#DC8CC3")))
 '(org-fontify-done-headline t)
 '(org-fontify-quote-and-verse-blocks t)
 '(org-fontify-whole-heading-line t)
 '(org-support-shift-select t)
 '(package-selected-packages
   (quote
    (gruvbox-theme ido-completing-read+ ggtags centered-cursor-mode zenburn-theme spotify rtags popup-switcher markdown-mode magit lua-mode langtool ibuffer-projectile helm-gtags helm-git go-mode git-gutter-fringe gist flycheck flx-ido flim f dockerfile-mode discover debian-changelog-mode dash-functional company-c-headers ag)))
 '(pdf-view-midnight-colors (quote ("#DCDCCC" . "#383838")))
 '(pos-tip-background-color "#3a3a3a")
 '(pos-tip-foreground-color "#9E9E9E")
 '(projectile-mode t nil (projectile))
 '(save-place-mode 1)
 '(scroll-bar-mode nil)
 '(scroll-conservatively 10000)
 '(scroll-margin 0)
 '(scroll-preserve-screen-position t)
 '(server-kill-new-buffers t)
 '(server-mode t)
 '(show-paren-mode t)
 '(split-height-threshold 200)
 '(split-width-threshold 140)
 '(tab-bar-mode t)
 '(tetris-x-colors
   [[229 192 123]
    [97 175 239]
    [209 154 102]
    [224 108 117]
    [152 195 121]
    [198 120 221]
    [86 182 194]])
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
 '(default ((t (:family "Envy Code R" :foundry "ENVY" :slant normal :weight normal :height 92 :width normal))))
 '(tab-bar-tab ((t (:inherit tab-bar :box (:line-width 1 :style released-button))))))
