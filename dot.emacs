;;; pacakge --- Troglobit Enterprise ¯\_(ツ)_/¯ Emacs                -*-Emacs-LISP-*-
;;
;; Copyright (c) 2009-2022  Joachim Wiberg <troglobit@gmail.com>
;;
;; Permission to use, copy, modify, and/or distribute this software for any
;; purpose with or without fee is hereby granted, provided that the above
;; copyright notice and this permission notice appear in all copies.
;;
;; THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
;; WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
;; MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
;; ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
;; WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
;; ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
;; OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
;;
;;; Commentary:
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
;; Save it as ~/.emacs (dot.emacs in your home directory) to activate
;; and then (re)start your Emacs to start populating ~/.emacs.d/ ...
;;
;;; Code:

(setq initial-scratch-message    ";; Welcome to Troglobit Enterprise ¯\\_(ツ)_/¯ Emacs!
;;\n\n")

;; Location for privately maintained packages.
(add-to-list 'load-path "~/.emacs.d/site-lisp")
(add-to-list 'load-path "~/.emacs.d/epresent")

;; Prevent accidental exit of Emacs
(fset 'yes-or-no-p 'y-or-n-p)
(setq confirm-kill-emacs (quote y-or-n-p))

;; Cleanup mode by default
(add-hook 'before-save-hook
          'delete-trailing-whitespace)

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
;; (autoload 'epresent-run "epresent")
;; (add-hook 'org-mode-hook
;; 	(function
;; 	 (lambda ()
;; 	   (setq truncate-lines nil)
;; 	   (setq word-wrap t)
;; 	   (define-key org-mode-map [f3]
;; 	     'epresent-run)
;; 	   )))

;; Always enable ggtags for C projects
(add-hook 'c-mode-common-hook
          (lambda ()
            (when (derived-mode-p 'c-mode 'c++-mode 'java-mode 'python-mode)
              (ggtags-mode 1))))

;; When saving a file that starts with #!, make it executable.
(defun my-make-file-executable-maybe ()
  "Make hash-bang file executable unless it's a Python file."
  (unless (file-name-extension buffer-file-name ".py")
    (executable-make-buffer-file-executable-if-script-p)))

(add-hook 'after-save-hook
          'my-make-file-executable-maybe)

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

;; Email setup, for notmuch
;; From https://github.com/admiralakber/simplemacs/blob/master/modules/notmuch.org
;; Rembmer: sudo apt install mailscripts
;; to get notmuch-extract-patch
(use-package notmuch
  :init
  (setq message-directory "~/mail")
  (setq send-mail-function 'sendmail-send-it)
  ;; Send from correct email account
  (setq message-sendmail-f-is-eval 't)
  (setq message-sendmail-extra-arguments '("--read-envelope-from"))
  (setq message-citation-line-format "On %a, %b %d, %Y at %R, %f wrote:")
  (setq message-citation-line-function 'message-insert-formatted-citation-line)
  (setq mail-user-agent 'message-user-agent)
  (setq message-send-mail-function 'smtpmail-send-it
	smtpmail-starttls-credentials '(("smtp.gmail.com" 587 nil nil))
	smtpmail-auth-credentials '(("smtp.gmail.com" 587 "troglobit@gmail.com" nil))
	smtpmail-default-smtp-server "smtp.gmail.com"
	smtpmail-smtp-server "smtp.gmail.com"
	smtpmail-smtp-service 587
	starttls-use-gnutls t)
  (setq mail-specify-envelope-from 't)
  (setq mail-envelope-from 'header)
  (setq message-sendmail-envelope-from 'header)
  ;; Setting proper from, fixes i-did-not-set--mail-host-address--so-tickle-me
  (setq mail-host-address "gmail.com")
  (setq user-full-name "Joachim Wiberg")
  (setq user-mail-address "troglobit@gmail.com")
  :config
  (setq notmuch-show-logo nil)
  ;; Writing email
  (setq message-default-mail-headers "Cc: \nBcc: \n") ;; Always show BCC
  (setq notmuch-always-prompt-for-sender 't)
  ;; PGP Encryption
  (add-hook 'message-setup-hook 'mml-secure-sign-pgpmime)
  (setq notmuch-crypto-process-mime t))

;; Easy Hugo -- https://github.com/masasam/emacs-easy-hugo
(use-package easy-hugo
	     :init
	     (setq easy-hugo-basedir "~/Troglobit/Bloggy/")
	     (setq easy-hugo-url "http://troglobit.com")
	     (setq easy-hugo-sshdomain "fra.troglobit.com:222")
	     (setq easy-hugo-root "/var/www/fra.troglobit.com/")
	     (setq easy-hugo-previewtime "300")
	     :bind ("C-c C-e" . easy-hugo))

(use-package gruvbox-theme
  :init  (load-theme 'gruvbox-dark-medium t))

;; Emacs Code Browser -- http://ecb.sourceforge.net/
;; (use-package ecb
;;   :init
;;   (setq ecb-examples-bufferinfo-buffer-name nil)
;;   (setq ecb-tip-of-the-day nil)
;;   (progn
;;     (require 'ecb)))

;;
(use-package ido-completing-read+)

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

(use-package yaml-mode
  :mode (("\\.yml$"  . yaml-mode)
	 ("\\.yaml$" . yaml-mode)))

(use-package yang-mode)

(use-package dts-mode
  :mode (("\\.dts$"  . dts-mode)
	 ("\\.dtsi$" . dts-mode)
	 ("\\.dts-*$" . dts-mode)))

(use-package markdown-mode
  :commands (markdown-mode gfm-mode)
  :hook (markdown-mode . flyspell-mode)
  :mode (("README\\.md$'" . gfm-mode)
         ("\\.md$'" . markdown-mode)
         ("\\.markdown$'" . markdown-mode))
  :config (setq markdown-command (concat "pandoc -t html5 --standalone --template "
					 (expand-file-name "~/.emacs.d/template.html"))))

(use-package markdown-toc)
(use-package pandoc-mode)
(use-package gh-md)
(use-package dpkg-dev-el)

;; Auto-detect indent settings
(use-package dtrt-indent)

(use-package flycheck
  :init (global-flycheck-mode))

(use-package flycheck-clang-tidy
  :after flycheck
  :hook (flycheck-mode . flycheck-clang-tidy-setup))

;; GNU Global Tags
(use-package ggtags
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

;; Center text on screen, helps with ggtags lookup but feels a bit weird.
;; https://emacs.stackexchange.com/questions/36094/using-ggtags-tag-lookup-along-with-centered-cursor-mode
;; (setq scroll-preserve-screen-position t
;;       scroll-conservatively 0
;;       maximum-scroll-margin 0.5
;;       scroll-margin 99999)

;; Complete anything (company)
(use-package company
  :init (global-company-mode)
  :config
  (progn
    ;; Use Company for completion
    (bind-key [remap completion-at-point] #'company-complete company-mode-map)

    (setq company-tooltip-align-annotations t
          ;; Easy navigation to candidates with M-<n>
          company-show-quick-access t
	  company-abort-manual-when-too-short t
	  company-insertion-on-trigger 'company-explicit-action-p)
    (setq company-dabbrev-downcase nil))
  :diminish company-mode)

(use-package company-quickhelp          ; Documentation popups for Company
  :init (add-hook 'global-company-mode-hook #'company-quickhelp-mode))

(use-package company-c-headers
  :init
  (with-eval-after-load 'company
    (add-to-list 'company-backends 'company-c-headers)))

;; Honor .editorconfig files in projects, for details, see
;; https://github.com/editorconfig/editorconfig-emacs
(use-package editorconfig
  :diminish editorconfig-mode
  :config
  (editorconfig-mode 1))

;; It's useful to be able to restart emacs from inside emacs.
(use-package restart-emacs)

;; Project Handling
(use-package projectile
  :init
  :bind-keymap (("C-c p" . projectile-command-map))
  :config (projectile-mode t))

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
		   magit-define-global-key-bindings t
		   magit-completing-read-function 'magit-ido-completing-read
		   global-magit-file-mode 't
		   magit-commit-arguments (quote ("--signoff"))
		   magit-commit-signoff 't
		   magit-diff-refine-hunk (quote all)
		   magit-diff-use-overlays nil))

(use-package magit-popup)

;; (use-package magit-org-todos
;;   :after magit
;;   :config
;;   (magit-org-todos-autoinsert)
;;   (setq magit-org-todos-filename "TODO.org"))

;; PACKAGES BROKEN ON EMACS 30/31 as of 2024-03-03
;; (use-package forge
;;   :after magit)
;; (use-package git-timemachine)

(use-package diminish
  :init (require 'diminish)
  :config
  (diminish 'projectile-mode)
  (diminish 'flycheck-mode)
  (diminish 'eldoc-mode)
  (diminish 'abbrev-mode))

;; Powerline is a neat modeline replacement
;; (use-package powerline
;;   :init (powerline-default-theme))

;; (use-package doom-modeline
;;   :init (doom-modeline-mode 1))

;; Directory sidebar
;; (use-package treemacs
;;   :bind
;;   (("C-c t" . treemacs)
;;    ("s-a" . treemacs)))

;; (use-package treemacs
;;   :defer t
;;   :init
;;   (with-eval-after-load 'winum
;;     (define-key winum-keymap (kbd "M-0") #'treemacs-select-window))
;;   :config
;;   (progn
;;     (setq treemacs-collapse-dirs                   (if treemacs-python-executable 3 0)
;;           treemacs-deferred-git-apply-delay        0.5
;;           treemacs-directory-name-transformer      #'identity
;;           treemacs-display-in-side-window          t
;;           treemacs-eldoc-display                   'simple
;;           treemacs-file-event-delay                2000
;;           treemacs-file-extension-regex            treemacs-last-period-regex-value
;;           treemacs-file-follow-delay               0.2
;;           treemacs-file-name-transformer           #'identity
;;           treemacs-follow-after-init               t
;;           treemacs-expand-after-init               t
;;           treemacs-find-workspace-method           'find-for-file-or-pick-first
;;           treemacs-git-command-pipe                ""
;;           treemacs-goto-tag-strategy               'refetch-index
;;           treemacs-header-scroll-indicators        '(nil . "^^^^^^")
;;           treemacs-hide-dot-git-directory          t
;;           treemacs-indentation                     2
;;           treemacs-indentation-string              " "
;;           treemacs-is-never-other-window           t
;;           treemacs-max-git-entries                 5000
;;           treemacs-missing-project-action          'ask
;;           treemacs-move-forward-on-expand          nil
;;           treemacs-no-png-images                   nil
;;           treemacs-no-delete-other-windows         t
;;           treemacs-project-follow-cleanup          nil
;;           treemacs-persist-file                    (expand-file-name ".cache/treemacs-persist" user-emacs-directory)
;;           treemacs-position                        'left
;;           treemacs-read-string-input               'from-child-frame
;;           treemacs-recenter-distance               0.1
;;           treemacs-recenter-after-file-follow      nil
;;           treemacs-recenter-after-tag-follow       nil
;;           treemacs-recenter-after-project-jump     'always
;;           treemacs-recenter-after-project-expand   'on-distance
;;           treemacs-litter-directories              '("/node_modules" "/.venv" "/.cask")
;;           treemacs-project-follow-into-home        nil
;;           treemacs-show-cursor                     nil
;;           treemacs-show-hidden-files               t
;;           treemacs-silent-filewatch                nil
;;           treemacs-silent-refresh                  nil
;;           treemacs-sorting                         'alphabetic-asc
;;           treemacs-select-when-already-in-treemacs 'move-back
;;           treemacs-space-between-root-nodes        t
;;           treemacs-tag-follow-cleanup              t
;;           treemacs-tag-follow-delay                1.5
;;           treemacs-text-scale                      nil
;;           treemacs-user-mode-line-format           nil
;;           treemacs-user-header-line-format         nil
;;           treemacs-wide-toggle-width               70
;;           treemacs-width                           35
;;           treemacs-width-increment                 1
;;           treemacs-width-is-initially-locked       t
;;           treemacs-workspace-switch-cleanup        nil)

;;     ;; The default width and height of the icons is 22 pixels. If you are
;;     ;; using a Hi-DPI display, uncomment this to double the icon size.
;;     ;;(treemacs-resize-icons 44)

;;     (treemacs-follow-mode t)
;;     (treemacs-filewatch-mode t)
;;     (treemacs-fringe-indicator-mode 'always)
;;     (when treemacs-python-executable
;;       (treemacs-git-commit-diff-mode t))

;;     (pcase (cons (not (null (executable-find "git")))
;;                  (not (null treemacs-python-executable)))
;;       (`(t . t)
;;        (treemacs-git-mode 'deferred))
;;       (`(t . _)
;;        (treemacs-git-mode 'simple)))

;;     (treemacs-hide-gitignored-files-mode nil))
;;   :bind
;;   (:map global-map
;;         ("M-0"       . treemacs-select-window)
;;         ("C-x t 1"   . treemacs-delete-other-windows)
;;         ("C-x t t"   . treemacs)
;;         ("C-x t d"   . treemacs-select-directory)
;;         ("C-x t B"   . treemacs-bookmark)
;;         ("C-x t C-t" . treemacs-find-file)
;;         ("C-x t M-t" . treemacs-find-tag)))

;; (use-package treemacs-projectile
;;   :after (treemacs projectile))

;; (use-package treemacs-icons-dired
;;   :hook (dired-mode . treemacs-icons-dired-enable-once))

;; (use-package treemacs-magit
;;   :after (treemacs magit))

;; (use-package treemacs-tab-bar
;;   :after (treemacs dash)
;;   :config (treemacs-set-scope-type 'Tabs))

;; (defun treemacs-styling-hooks ()
;;   "Hide modeline and line numbers in treemacs window."
;;   (setq-local mode-line-format nil)
;;   (display-line-numbers-mode -1))

;; (add-hook 'treemacs-mode-hook 'treemacs-styling-hooks)

(use-package dash)

(use-package highlight-indent-guides
  :init (highlight-indent-guides-mode t)
  :hook (prog-mode . highlight-indent-guides-mode))

;; (use-package balanced-windows
;;   :config (balanced-windows-mode))

;; (use-package centaur-tabs
;;   :demand
;;   :config
;;   (centaur-tabs-mode t)
;;   :bind
;;   ("C-<prior>" . centaur-tabs-backward)
;;   ("C-<next>" . centaur-tabs-forward))

;; Nyan Cat buffer position :-)
;; (use-package nyan-mode
;;   :init (nyan-mode t))

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
(setq tool-bar-mode nil
      tooltip-mode nil
      scroll-bar-mode nil
      tab-bar-mode t
      tab-bar-select-tab-modifiers '(meta))

;; Add fringes
(setq-default indicate-buffer-boundaries 'right)
(setq-default indicate-empty-lines t)
(setq-default frame-title-format '("%b - %F"))

;; Make modeline prettier
(set-face-attribute 'mode-line nil :box nil)
(set-face-attribute 'mode-line-inactive nil :box nil)
(set-face-attribute 'mode-line-highlight nil :box nil)

;; Prettier scroll bar than toolkit default
;; '(global-yascroll-bar-mode t)
;; '(yascroll:delay-to-hide nil)
;; '(yascroll:scroll-bar (quote (right-fringe left-fringe text-area)))

;; Change default theme
;;(add-to-list 'custom-theme-load-path "~/.emacs.d/themes/")
;;(load-theme 'radiance t)
;;(load-theme 'github t)
;;(load-theme 'troglobit-dark t)
;;(load-theme 'darcula t)

;; Show colors in Emacs when color codes are listed
;;(rainbow-mode t)

;; ANSI colors in Emacs compilation buffer
;; http://stackoverflow.com/questions/3072648/cucumbers-ansi-colors-messing-up-emacs-compilation-buffer
(ignore-errors
  (require 'ansi-color)
  (defun my-colorize-compilation-buffer ()
    (when (eq major-mode 'compilation-mode)
      (ansi-color-apply-on-region compilation-filter-start (point-max))))
  (add-hook 'compilation-filter-hook 'my-colorize-compilation-buffer))

;; Pimp org-files
;; https://thraxys.wordpress.com/2016/01/14/pimp-up-your-org-agenda/
;; (use-package org-bullets
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
;; S-F12   Toggle Tab bar
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
  (global-set-key [M-f12] 'scroll-bar-mode)
  (global-set-key [S-f12] 'tab-bar-mode))

(global-set-key (kbd "M-#") 'calculator)

(global-set-key (kbd "C-+") 'text-scale-increase)
(global-set-key (kbd "C--") 'text-scale-decrease)

;; Drop tab-bar-mode default bindings
(global-unset-key '[C-S-iso-lefttab])
(global-unset-key '[C-tab])
(global-set-key '[C-tab] 'other-window)
(global-set-key '[C-iso-lefttab] 'other-window-backwards)

;; Moving around windows in a frame now that we have tabs
(global-set-key '[M-left] 'windmove-left)
(global-set-key '[M-right] 'windmove-right)
(global-set-key '[M-up] 'windmove-up)
(global-set-key '[M-down] 'windmove-down)

;; For Emacs 27.1 tab-bar-mode
(global-set-key '[C-next] 'tab-next)
(global-set-key '[C-prior] 'tab-previous)

(defun insert-file-header () (interactive)
  (insert "/* <FILE DESCRIPTION>\n")
  (insert " *\n")
  (insert " * Copyright (C) 2022  Addiva Elektronik AB\n")
  (insert " *\n")
  (insert " * Author: Joachim Wiberg <joachim.wiberg@addiva.se>\n")
  (insert " *\n")
  (insert " * Description:\n")
  (insert " *\n")
  (insert " */\n"))

(defun insert-include-body () (interactive)
  (insert "/* <FILE DESCRIPTION>\n")
  (insert " *\n")
  (insert " * Copyright (C) 2022  Addiva Elektronik AB\n")
  (insert " *\n")
  (insert " * Author: Joachim Wiberg <joachim.wiberg@addiva.se>\n")
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
  (insert " * Copyright (c) 2022  Joachim Wiberg <troglobit@gmail.com>\n")
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
  (insert "Signed-off-by: Joachim Wiberg <joachim.wiberg@addiva.se>\n"))

(defun insert-signed-off () (interactive)
  (insert "Signed-off-by: Joachim Wiberg <troglobit@gmail.com>\n"))

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
;;(global-set-key (kbd "C-c m") 'magit-status)

(defun magit-toggle-scroll-to-top () (recenter-top-bottom 0))
(advice-add 'magit-toggle-section :after #'magit-toggle-scroll-to-top)

;; https://www.reddit.com/r/emacs/comments/qoslj5/automatic_text_size_adjustment_per_display/
;; https://github.com/WJCFerguson/textsize/
;; (use-package textsize
;;   :commands textsize-mode
;;   :init (textsize-mode))

;; Automatically raise new frames created with C-x 5 2
;;(select-frame-set-input-focus (window-frame (selected-window)))
(x-focus-frame nil)
;;(other-frame 0)

(defun count-buffers (&optional display-anyway)
  "Display or return the number of buffers."
  (interactive)
  (let ((buf-count (length (buffer-list))))
    (if (or (interactive-p) display-anyway)
    (message "%d buffers in this Emacs" buf-count)) buf-count))

;;(require 'flx-ido)
;;(ido-mode 1)
;;(ido-everywhere 1)
;;(flx-ido-mode 1)
;; disable ido faces to see flx highlights.
;;(setq ido-enable-flex-matching t)
;;(setq ido-use-faces nil)

(define-derived-mode fundamental-ansi-mode fundamental-mode "fundamental ansi"
  "Fundamental mode that understands ansi colors."
  (require 'ansi-color)
  (ansi-color-apply-on-region (point-min) (point-max)))

;; '(global-display-line-numbers-mode t)
;; '(x-stretch-cursor t)
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
   ["#424242" "#EF9A9A" "#C5E1A5" "#FFEE58" "#64B5F6" "#E1BEE7" "#80DEEA"
    "#E0E0E0"])
 '(auth-source-save-behavior nil)
 '(beacon-color "#ec4780")
 '(before-save-hook nil)
 '(c-default-style
   '((c-mode . "linux") (c++-mode . "linux") (java-mode . "java")
     (awk-mode . "awk") (other . "gnu")))
 '(c-require-final-newline '((c-mode . t) (c++-mode . t) (objc-mode . t)))
 '(column-number-mode t)
 '(company-abort-manual-when-too-short t)
 '(company-insertion-on-trigger ''company-explicit-action-p)
 '(compilation-message-face 'default)
 '(completion-ignored-extensions
   '(".o" "~" ".bin" ".lbin" ".so" ".a" ".ln" ".blg" ".bbl" ".elc" ".lof"
     ".glo" ".idx" ".lot" ".svn/" ".hg/" ".git/" ".bzr/" "CVS/"
     "_darcs/" "_MTN/" ".fmt" ".tfm" ".class" ".fas" ".lib" ".mem"
     ".x86f" ".sparcf" ".dfsl" ".pfsl" ".d64fsl" ".p64fsl" ".lx64fsl"
     ".lx32fsl" ".dx64fsl" ".dx32fsl" ".fx64fsl" ".fx32fsl" ".sx64fsl"
     ".sx32fsl" ".wx64fsl" ".wx32fsl" ".fasl" ".ufsl" ".fsl" ".dxl"
     ".lo" ".la" ".gmo" ".mo" ".toc" ".aux" ".cp" ".fn" ".ky" ".pg"
     ".tp" ".vr" ".cps" ".fns" ".kys" ".pgs" ".tps" ".vrs" ".pyc" ".pyo"
     ".d"))
 '(custom-enabled-themes '(gruvbox-dark-soft))
 '(custom-safe-themes
   '("871b064b53235facde040f6bdfa28d03d9f4b966d8ce28fb1725313731a2bcc8"
     "3e374bb5eb46eb59dbd92578cae54b16de138bc2e8a31a2451bf6fdb0f3fd81b"
     "19a2c0b92a6aa1580f1be2deb7b8a8e3a4857b6c6ccf522d00547878837267e7"
     "fa49766f2acb82e0097e7512ae4a1d6f4af4d6f4655a48170d0a00bcb7183970"
     "2ff9ac386eac4dffd77a33e93b0c8236bb376c5a5df62e36d4bfa821d56e4e20"
     "b1a691bb67bd8bd85b76998caf2386c9a7b2ac98a116534071364ed6489b695d"
     "78c4238956c3000f977300c8a079a3a8a8d4d9fee2e68bad91123b58a4aa8588"
     "6bdcff29f32f85a2d99f48377d6bfa362768e86189656f63adbf715ac5c1340b"
     "4eb6fa2ee436e943b168a0cd8eab11afc0752aebb5d974bba2b2ddc8910fca8f"
     "83e0376b5df8d6a3fbdfffb9fb0e8cf41a11799d9471293a810deb7586c131e6"
     "6b5c518d1c250a8ce17463b7e435e9e20faa84f3f7defba8b579d4f5925f60c1"
     "2b9dc43b786e36f68a9fd4b36dd050509a0e32fe3b0a803310661edb7402b8b6"
     "84d2f9eeb3f82d619ca4bfffe5f157282f4779732f48a5ac1484d94d5ff5b279"
     "a22f40b63f9bc0a69ebc8ba4fbc6b452a4e3f84b80590ba0a92b4ff599e53ad0"
     "585942bb24cab2d4b2f74977ac3ba6ddbd888e3776b9d2f993c5704aa8bb4739"
     "8f97d5ec8a774485296e366fdde6ff5589cf9e319a584b845b6f7fa788c9fa9a"
     "669e02142a56f63861288cc585bee81643ded48a19e36bfdf02b66d745bcc626"
     default))
 '(delete-active-region t)
 '(delete-selection-mode t)
 '(desktop-restore-in-current-display t)
 '(desktop-save-mode t)
 '(diff-switches "-u")
 '(display-line-numbers-type nil)
 '(ediff-merge-split-window-function 'split-window-horizontally)
 '(ediff-split-window-function 'split-window-horizontally)
 '(ediff-window-setup-function 'ediff-setup-windows-plain)
 '(fci-rule-color "#383838")
 '(fill-column 72)
 '(font-use-system-font nil)
 '(global-auto-revert-mode t)
 '(global-company-mode nil)
 '(global-prettify-symbols-mode nil)
 '(graphviz-dot-preview-extension "svg")
 '(highlight-indent-guides-auto-enabled t)
 '(highlight-indent-guides-method 'bitmap)
 '(highlight-symbol-colors
   '("#FFEE58" "#C5E1A5" "#80DEEA" "#64B5F6" "#E1BEE7" "#FFCC80"))
 '(highlight-symbol-foreground-color "#E0E0E0")
 '(highlight-tail-colors '(("#ec4780" . 0) ("#424242" . 100)))
 '(ido-mode 'both nil (ido))
 '(indicate-buffer-boundaries '((top . left) (bottom . right)))
 '(indicate-empty-lines t)
 '(inhibit-startup-screen t)
 '(ispell-dictionary "american")
 '(mouse-wheel-scroll-amount '(1 ((shift) . 1) ((control))))
 '(notmuch-saved-searches
   '((:name "inbox" :query "tag:inbox" :key "i")
     (:name "unread" :query "tag:unread" :key "u")
     (:name "flagged" :query "tag:flagged" :key "f")
     (:name "sent" :query "tag:sent" :key "t")
     (:name "drafts" :query "tag:draft" :key "d")
     (:name "all mail" :query "*" :key "a")
     (:name "important" :query "tag:important")
     (:name "me" :query "me and unread")))
 '(nrepl-message-colors
   '("#CC9393" "#DFAF8F" "#F0DFAF" "#7F9F7F" "#BFEBBF" "#93E0E3" "#94BFF3"
     "#DC8CC3"))
 '(org-fontify-done-headline t)
 '(org-fontify-quote-and-verse-blocks t)
 '(org-fontify-whole-heading-line t)
 '(org-support-shift-select t)
 '(package-selected-packages nil)
 '(pdf-view-midnight-colors '("#DCDCCC" . "#383838"))
 '(pos-tip-background-color "#3a3a3a")
 '(pos-tip-foreground-color "#9E9E9E")
 '(projectile-dynamic-mode-line nil)
 '(projectile-mode t nil (projectile))
 '(save-place-mode 1)
 '(scalable-fonts-allowed t)
 '(scroll-bar-mode nil)
 '(server-kill-new-buffers t)
 '(server-mode t)
 '(server-raise-frame t)
 '(sh-basic-offset 4)
 '(split-height-threshold 200)
 '(split-width-threshold 140)
 '(tab-bar-mode t)
 '(tetris-x-colors
   [[229 192 123] [97 175 239] [209 154 102] [224 108 117] [152 195 121]
    [198 120 221] [86 182 194]])
 '(text-scale-mode-step 1.4)
 '(tool-bar-mode nil)
 '(tramp-syntax 'default nil (tramp))
 '(uniquify-buffer-name-style 'forward nil (uniquify))
 '(vc-annotate-background "#2B2B2B")
 '(vc-annotate-color-map
   '((20 . "#BC8383") (40 . "#CC9393") (60 . "#DFAF8F") (80 . "#D0BF8F")
     (100 . "#E0CF9F") (120 . "#F0DFAF") (140 . "#5F7F5F")
     (160 . "#7F9F7F") (180 . "#8FB28F") (200 . "#9FC59F")
     (220 . "#AFD8AF") (240 . "#BFEBBF") (260 . "#93E0E3")
     (280 . "#6CA0A3") (300 . "#7CB8BB") (320 . "#8CD0D3")
     (340 . "#94BFF3") (360 . "#DC8CC3")))
 '(vc-annotate-very-old-color "#DC8CC3")
 '(vc-display-status nil)
 '(visible-bell t)
 '(which-function-mode t))

;; Envy Code R is usally a great font, other great fonts are
;; Source Code Pro and Inconsolata
(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(default ((t (:family "Source Code Pro" :foundry "ADBO" :slant normal :weight regular :height 98 :width normal)))))
