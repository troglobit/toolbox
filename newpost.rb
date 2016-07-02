#!/usr/bin/env ruby
# Create new Jekyll blog post
#
# Copyright (c) 2016  Joachim Nilsson <troglobit@gmail.com>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

unless ARGV[0]
  puts 'Usage: newpost "the post title"'
  exit(1)
end

dir    = "_posts"
title  = ARGV[0]
now    = Time.new()
name   = title.strip.downcase.gsub(/[^0-9A-Za-z.\-]/, '-')
prefix = now.strftime("%Y-%m-%d")
today  = now.strftime('%Y-%m-%d %H:%M:%S %z')
filenm = "#{prefix}-#{name}.markdown"
path   = File.join(dir, filenm)

header = <<-END
---
layout: post
title: "#{title}"
date: #{today}
comments: true
categories:
---

END

if !File.exists?(dir)
  puts "The blog post directory '#{dir}' does not exist."
  puts "Try running this command from the top directory of your Jekyll blog."
  exit(1)
end

if File.exists?(path)
  puts "#{path} already exists"
  exit(1)
end

File.open(path, 'w') do |f|
  f << header
  f.close
end

system("emacsclient", "-a vim", "-c", "+9", path)
system("git", "add", path)

