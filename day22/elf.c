#include "elf.h"

int elf32_validate(Elf32_Ehdr *hdr) {
  return hdr->e_ident[EI_MAG0] == ELFMAG0 && hdr->e_ident[EI_MAG1] == ELFMAG1 &&
         hdr->e_ident[EI_MAG2] == ELFMAG2 && hdr->e_ident[EI_MAG3] == ELFMAG3;
}

Elf32_Shdr *get_data_section(const char *buf, Elf32_Ehdr *elf_header) {
  for (int i = 0; i < elf_header->e_shnum; i++) {
    Elf32_Shdr *shdr =
        (Elf32_Shdr *)(buf + elf_header->e_shoff + sizeof(Elf32_Shdr) * i);

    if (shdr->sh_type != SHT_PROGBITS) {
      continue;
    }

    if ((shdr->sh_flags & SHF_WRITE) && (shdr->sh_flags & SHF_ALLOC)) {
      return shdr;
    }
  }

  return 0;
}
