append([], L2, L2).
append([H|T], L2, [H|R]):- append(T, L2, R).

reverse(L, RL):- rev(L, [], RL).
rev([], Stack, Stack).
rev([H|T], Stack, R):- rev(T, [H|Stack], R).


repeatList([], 0, _).
repeatList([P|XS], N, P):- N > 0, N1 is N - 1, repeatList(XS, N1, P). 

field(F, N):- repeatList(Lf, N, '>'), repeatList(Rf, N, '<'),  append(Lf, ['_'], M), append(M, Rf, F).
isSolved(F, N):- repeatList(Lf, N, '<'), repeatList(Rf, N, '>'),  append(Lf, ['_'], M), append(M, Rf, F).

solution(F, [F], N):- isSolved(F, N).
solution(F, [F|S], N):- append(L,['_' | ['<' | R]],F), append(L,['<' | ['_' | R]],F1), solution(F1, S, N).
solution(F, [F|S], N):- append(L,['_' | [C | ['<' | R]]],F), append(L,['<' | [C | ['_' | R]]],F1), solution(F1, S, N).
solution(F, [F|S], N):- append(L,['_' | R],F), append(L1, ['>'], L), append(L1, ['_'], L2), append(L2,['>' | R],F1), solution(F1, S, N).
solution(F, [F|S], N):- append(L,['_' | R],F), append(L1, ['>' | [C]], L), append(L1, ['_' | [C]], L2), append(L2,['>' | R],F1), solution(F1, S, N).

frogLeap(S, N):- field(F, N), solution(F, S, N).