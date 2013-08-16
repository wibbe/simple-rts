
# Input bindings

# Camera movement
input:bind2 W { player:panForward 1 } { player:panForward 0 }
input:bind2 S { player:panForward -1 } { player:panForward 0 }
input:bind2 A { player:panSideways -1 } { player:panSideways 0 }
input:bind2 D { player:panSideways 1 } { player:panSideways 0 }

input:bind ESCAPE quit

input:bind2 MOUSE_LEFT {
  puts DOWN([input:mouseX]x[input:mouseY])
} {
  puts UP([input:mouseX]x[input:mouseY])
}
