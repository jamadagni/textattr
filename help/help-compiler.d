module help_compiler;

import textattr;
import std.stdio, std.string, std.regex;

auto applyTa(T)(T text) if (is(T: const(char)[]))
{
    static auto inlineRE = ctRegex!`\$\(ta (.*?)\)`;
    return text.replaceAll!(match => ta(match[1]))(inlineRE);
}

void main(string [] args)
{
    assert(args.length == 2);
    auto W = ta("w"), F = ta("f");
    foreach (line; File(args[1]).byLine())
        line.replace("${W}", W).replace("${F}", F).applyTa.writeln;
}
